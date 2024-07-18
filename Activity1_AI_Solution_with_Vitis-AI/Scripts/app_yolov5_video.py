import xir
import vitis_ai_library
import numpy as np
from PIL import Image
from originals.postprocessing import postprocessing
from originals.non_maximum_suppression import non_max_suppression
from torch import tensor
import cv2
import random
import json
from time import time
import torchvision.transforms.functional as F
from utils.padding import SquarePad

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
path_yolo_attributes = "/home/ubuntu/Vitis-AI/DEV/models/pt_detection_yolov5_stable/model_compiled/YoloV5s_workshop.attributes" #"/home/ubuntu/Vitis-AI/DEV/models/pt_detection_yolov5_2.5/model_compiled/YoloV5s_workshop.attributes"
path_image = "/home/ubuntu/Vitis-AI/DEV/inputs/zidane.jpg"

# Step-2) obtain model graph from serialized model file (xmodel)
graph = xir.Graph.deserialize(path_model)

# Step-3) create graph runner instance for obtained graph
dpu_runner = vitis_ai_library.GraphRunner.create_graph_runner(graph)

# Step-4) get lists of input and output tensor buffers
list_input_tensor_buffers = dpu_runner.get_inputs()
list_output_tensor_buffers = dpu_runner.get_outputs()

# Step-5) get input and output shapes of input/output tensors
# (use index-0 directly since there is only one input and one output in current graph)
list_input_shapes = []
for inp_tens in list_input_tensor_buffers:
    list_input_shapes.append(inp_tens.get_tensor().dims)
list_output_shapes = []
for out_tens in list_output_tensor_buffers:
    list_output_shapes.append(out_tens.get_tensor().dims)

# Step-6) get input & output scaler value for each input tensor
# (use index-0 directly since there is only one input)
fixpos_input = list_input_tensor_buffers[0].get_tensor().get_attr("fix_point")
scaler_input = 2**fixpos_input

# Step-7) initialize output data
list_output_data = []
for shape in list_output_shapes:
    array = np.empty((shape), dtype=np.float32)
    array = array.transpose(0, 3, 1, 2)
    list_output_data.append(array)


# Step-8) Select video source
capture = cv2.VideoCapture(0)

# Step-9) Reading classes list from json
with open("COCO_classes.json") as json_file:
    dict_classes = json.load(json_file)

## Step-10) Define a color for each class
color_map = {
    class_name: (random.randint(0, 255), random.randint(0, 255), random.randint(0, 255))
    for class_name in dict_classes.keys()
}

padding = SquarePad(capture.read()[1])


# DYNAMIC START
while True:

    t1 = time()
    ret, frame = capture.read()

    # PREPROCESSING
    frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    frame_padded = padding(frame)
    frame_padded = cv2.resize(frame_padded, (640, 640))
    frame_norm = (frame_padded / 255.0).astype(np.float32)

    # Step-8) Preprocess input and initialize input data (SAME ORDER WITH INPUT TENSOR BUFFERS)
    list_input_data = []
    list_input_data.append((frame_norm * scaler_input).astype(np.int8))

    tdpu1 = time()
    # Step-10) Run graph runner instance
    job_id = dpu_runner.execute_async(list_input_data, list_output_data)
    dpu_runner.wait(job_id)
    tdpu2 = time()

    # Step-11) POST-PROCESSING: get predictions from output buffer
    list_predictions = []
    for i, data in enumerate(list_output_data):
        list_predictions.append(tensor(data))

    t_postp_1 = time()
    preds_processed = postprocessing(list_predictions, path_yolo_attributes)
    preds_nms = non_max_suppression(preds_processed, 0.2)
    t_postp_2 = time()

    # POSTPROCESSING
    frame = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)
    for elm in preds_nms[0]:
        #t_bbox_1 = time()
        x1, y1, x2, y2, conf, clas = list(elm)
        # cv2.rectangle(frame, (int(x1),int(y1)), (int(x2),int(y2)), (255,0,0), 2)
        cv2.rectangle(
            frame, (int(x1), int(y1)), (int(x2), int(y2)), color_map[str(int(clas))], 2
        )
        draw_text(
            frame,
            text=dict_classes[str(int(clas))]
            + "("
            + str(round(float(conf) * 100, 2))
            + "%)",
            font_scale=1,
            pos=(int(x1), int(y1)),
            text_color_bg=color_map[str(int(clas))],
        )

    t2 = time()
    t_diff = round(t2 - t1, 4)
    t_diff_dpu = round(tdpu2 - tdpu1, 4)
    t_diff_postp = round(t_postp_2 - t_postp_1, 4)
    fps = 1 / t_diff

    frame_preds = cv2.putText(frame.copy(), f"fps: {round(fps)}", (15, 40), cv2.FONT_HERSHEY_PLAIN, 2, (255,0,0), 2, cv2.LINE_AA)
    frame_preds = padding.remove(frame_preds)
    frame_preds = cv2.resize(frame_preds, (1080, 1080))

    cv2.namedWindow("YoloV5s-EmpaElectronics", cv2.WINDOW_FREERATIO)
    cv2.imshow("YoloV5s-EmpaElectronics", frame_preds)
    
    # print(
    #     f"FPS: {round(fps,4)} s, TotalExecTime: {t_diff*1000} ms, DPUExecTime: {t_diff_dpu*1000} ms, PostProcExecTime: {t_diff_postp*1000} ms"
    # )

    if cv2.waitKey(25) & 0xFF == ord("q"):
        break


capture.release()
cv2.destroyAllWindows()