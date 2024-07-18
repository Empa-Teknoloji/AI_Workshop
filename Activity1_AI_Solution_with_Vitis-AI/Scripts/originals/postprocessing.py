import torch
import pkg_resources as pkg


def check_version(current="0.0.0", minimum="0.0.0", name="version ", pinned=False, hard=False, verbose=False):
    # Check version vs. required version
    current, minimum = (pkg.parse_version(x) for x in (current, minimum))
    result = (current == minimum) if pinned else (current >= minimum)  # bool
    s = f"WARNING ⚠️ {name}{minimum} is required by YOLOv5, but {name}{current} is currently installed"  # string
    # if hard:
    #     assert result, emojis(s)  # assert min requirements met
    return result


def write_model_attr(model):
    head = model.model.model[-1]
    dict_attr = dict()
    dict_attr['nl'] = head.nl
    dict_attr['na'] = head.na
    dict_attr['no'] = head.no
    dict_attr['nc'] = head.nc
    dict_attr['dynamic'] = head.dynamic
    dict_attr['grid'] = head.grid
    dict_attr['anchor_grid'] = head.anchor_grid
    dict_attr['stride'] = head.stride
    dict_attr['anchors'] = head.anchors

    torch.save(dict_attr, "yolov5.attributes")

def read_model_attr(path_attributes):
    dict_attr = torch.load(path_attributes, map_location=torch.device('cpu'))
    return [dict_attr['nl'], dict_attr['na'], dict_attr['no'], dict_attr['nc']], [dict_attr['dynamic'], dict_attr['anchors'], dict_attr['stride'], dict_attr['grid'], dict_attr['anchor_grid']]


def _make_grid(nx=20, ny=20, i=0, torch_1_10=check_version(torch.__version__, "1.10.0"), na=None, anchors=None, stride=None):
        d = anchors[i].device
        t = anchors[i].dtype
        shape = 1, na, ny, nx, 2  # grid shape
        y, x = torch.arange(ny, device=d, dtype=t), torch.arange(nx, device=d, dtype=t)
        yv, xv = (torch.meshgrid(y, x, indexing="ij") if torch_1_10 else torch.meshgrid(y, x))  # torch>=0.7 compatibility
        grid = (torch.stack((xv, yv), 2).expand(shape) - 0.5).float()  # add grid offset, i.e. y = 2.0 * x - 0.5
        anchor_grid = ((anchors[i].clone() * stride[i]).view((1, na, 1, 1, 2)).expand(shape)).float()
        return grid, anchor_grid

def postprocessing(x, path_attr):

    [nl, na, no, nc], [dynamic, anchors, stride, grid, anchor_grid] = read_model_attr(path_attr)

    z = []  # inference output
    for i in range(nl):
        # print(x[i].shape)
        bs, _, ny, nx = x[i].shape  # x(bs,255,20,20) to x(bs,3,20,20,85)
        # print("ny, nx:", ny, nx, bs)
        x[i] = x[i].view(bs, na, no, ny, nx).permute(0, 1, 3, 4, 2).contiguous()

        if dynamic or grid[i].shape[2:4] != x[i].shape[2:4]:
            grid[i], anchor_grid[i] = _make_grid(nx, ny, i, na=na, anchors=anchors, stride=stride)

        # Detect (boxes only)
        xy, wh, conf = x[i].sigmoid().split((2, 2, nc + 1), 4)
        xy = (xy * 2 + grid[i]) * stride[i]  # xy
        wh = (wh * 2) ** 2 * anchor_grid[i]  # wh
        y = torch.cat((xy, wh, conf), 4)
        z.append(y.view(bs, na * nx * ny, no))

    return (torch.cat(z, 1), x)