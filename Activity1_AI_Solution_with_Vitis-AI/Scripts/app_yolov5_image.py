import xir
import vitis_ai_library
import numpy as np
from PIL import Image
from originals.postprocessing import postprocessing
from originals.non_maximum_suppression import non_max_suppression
from torch import tensor
from pathlib import Path
from utils.padding import SquarePad
import cv2
import json
import random




def read_classes_json(path_json):
    with open(path_json) as json_file:
        dict_classes = json.load(json_file)
    return dict_classes

def draw_text(
    img,
    text,
    font=cv2.FONT_HERSHEY_PLAIN,
    pos=(0, 0),
    font_scale=1,
    font_thickness=1,
    text_color=(255, 255, 255),
    text_color_bg=(0, 0, 0),
):

    x, y = pos
    text_size, _ = cv2.getTextSize(text, font, font_scale, font_thickness)
    text_w, text_h = text_size
    cv2.rectangle(img, pos, (x + text_w, y + text_h), text_color_bg, -1)
    cv2.putText(
        img,
        text,
        (x, y + text_h + font_scale - 1),
        font,
        font_scale,
        text_color,
        font_thickness,
    )

    return text_size

# Step-1) define xmodel path
path_model = "/home/ubuntu/Vitis-AI/DEV/models/pt_detection_yolov5_stable/model_compiled/YoloV5s.xmodel"
path_yolo_attributes = "/home/ubuntu/Vitis-AI/DEV/models/pt_detection_yolov5_stable/model_compiled/YoloV5s_workshop.attributes"

# Step-2) PREPROCESSING: preprocess input data
path_image = "/home/ubuntu/Vitis-AI/DEV/inputs/kitchen.jpg"
image_orig = np.array(Image.open(path_image)) # .resize((640, 640))
image_orig = cv2.cvtColor(image_orig, cv2.COLOR_BGR2RGB)
padding = SquarePad(image_orig)
image_padded = padding(image_orig)
image_padded = cv2.resize(image_padded, (640, 640))
image_norm = (image_padded / 255.0).astype(np.float32)

image_name, image_ext = str(Path(path_image).name).split(".")
# write input image
cv2.imwrite(f"input_{image_name}.{image_ext}", image_padded)

# Step-3) obtain model graph from serialized model file (xmodel)
graph = xir.Graph.deserialize(path_model)

# Step-4) create graph runner instance for obtained graph
dpu_runner = vitis_ai_library.GraphRunner.create_graph_runner(graph)

# Step-5) get lists of input and output tensor buffers
list_input_tensor_buffers = dpu_runner.get_inputs()
list_output_tensor_buffers = dpu_runner.get_outputs()

# Step-6) get input and output shapes of input/output tensors
# (use index-0 directly since there is only one input and one output in current graph)
list_input_shapes = []
for inp_tens in list_input_tensor_buffers:
    list_input_shapes.append(inp_tens.get_tensor().dims)
list_output_shapes = []
for out_tens in list_output_tensor_buffers:
    list_output_shapes.append(out_tens.get_tensor().dims)
    
# Step-7) get input & output scaler value for each input tensor
# (use index-0 directly since there is only one input)
fixpos = list_input_tensor_buffers[0].get_tensor().get_attr("fix_point")
scaler_input = 2**fixpos

# Step-8) Preprocess input and initialize input data (SAME ORDER WITH INPUT TENSOR BUFFERS)
list_input_data = []
list_input_data.append((image_norm * scaler_input).astype(np.int8))

# Step-9) Reading classes list from json
dict_classes = read_classes_json("COCO_classes.json")

## Step-10) Define a color for each class
color_map = {
    class_name: (random.randint(0, 255), random.randint(0, 255), random.randint(0, 255))
    for class_name in dict_classes.keys()
}

# Step-11) initialize output data
list_output_data = []
for shape in list_output_shapes:
    array = np.empty((shape), dtype=np.float32)
    array = array.transpose(0,3,1,2)
    list_output_data.append(array)

# Step-12) Run graph runner instance
job_id = dpu_runner.execute_async(list_input_data, list_output_data)
dpu_runner.wait(job_id)

# Step-13) POST-PROCESSING: get predictions from output buffer
for i, data in enumerate(list_output_data):
    list_output_data[i] = tensor(data)
preds_processed = postprocessing(list_output_data, path_yolo_attributes)
#print(f" -- Predictions before NMS:\n{preds_processed}")
preds_nms = non_max_suppression(preds_processed, 0.40)
print(f"----- Predictions after NMS:\n{preds_nms}")

# IMAGE SAVE
image_preds = cv2.cvtColor(np.array(image_padded), cv2.COLOR_RGB2BGR)
# image_preds = cv2.resize(image_preds, (960, 540))
for elm in preds_nms[0]:
    x1, y1, x2, y2, conf, clas = list(elm)
    cv2.rectangle(image_preds, (int(x1),int(y1)), (int(x2),int(y2)), color_map[str(int(clas))], 2)
    draw_text(
            image_preds,
            text=dict_classes[str(int(clas))]
            + "("
            + str(round(float(conf) * 100, 2))
            + "%)",
            font_scale=1,
            pos=(int(x1), int(y1)),
            text_color_bg=color_map[str(int(clas))],
        )
image_name, image_ext = str(Path(path_image).name).split(".")
image_preds = padding.remove(image_preds)
image_preds = cv2.cvtColor(image_preds, cv2.COLOR_BGR2RGB)
cv2.imwrite(f"output_{image_name}.{image_ext}", image_preds)