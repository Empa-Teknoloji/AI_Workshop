import numpy as np
import torchvision.transforms.functional as F
from PIL import Image

class SquarePad:
    def __init__(self, image):
        self.h, self.w, self.c = image.shape
        list_dims = [self.h, self.w]
        max_wh = np.max(list_dims)
        pad_horizontal = int((max_wh - self.w) / 2)     # horizontal padding value
        pad_vertical = int((max_wh - self.h) / 2)       # vertical padding value
        if self.h == self.w:
            self.pad_dim = None
            self.pad_ratio = None
        elif np.argmax(list_dims) == 1:
            self.pad_dim = 0
            self.pad_ratio = pad_vertical / max_wh
        elif np.argmax(list_dims) == 0:
            self.pad_dim = 1
            self.pad_ratio = pad_horizontal / max_wh
        self.padding = (pad_horizontal, pad_vertical, pad_horizontal, pad_vertical)
    
    def __call__(self, image):
        if self.pad_dim is not None:
            image_padded = np.array(F.pad(Image.fromarray(image), self.padding, 0, 'constant'))
            return image_padded
        return image

    def remove(self, image):
        pad_horizontal, pad_vertical = self.padding[:2]
        h, w, c = image.shape
        pad_horizontal = int(self.pad_ratio * w)
        pad_vertical = int(self.pad_ratio * h)
        if self.pad_dim == 0:
            return image[pad_vertical:-pad_vertical]
        elif self.pad_dim == 1:
            return image[:, pad_horizontal:-pad_horizontal]
        else:
            return image