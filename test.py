# from uwimg import *
# im = load_image("data/dogsmall.jpg")
# a = nn_resize(im, im.w*4, im.h*4)
# save_image(a, "dog4x-nn")

# from uwimg import *
# im = load_image("data/dogsmall.jpg")
# a = bilinear_resize(im, im.w*4, im.h*4)
# save_image(a, "dog4x-bl")

# from uwimg import *
# im = load_image("data/dog.jpg")
# a = nn_resize(im, im.w//7, im.h//7)
# save_image(a, "dog7th-bl")

from uwimg import *
im = load_image("data/dog.jpg")
# f = make_box_filter(7)
f = make_sharpen_filter()
blur = convolve_image(im, f, 1)
save_image(blur, "dog-box7")

# from uwimg import *
# im = load_image("data/dog.jpg")
# f = make_box_filter(7)
# blur = convolve_image(im, f, 1)
# thumb = nn_resize(blur, blur.w//7, blur.h//7)
# save_image(thumb, "dogthumb")

# from uwimg import *
# im = load_image("data/dog.jpg")
# f = make_gaussian_filter(2)
# blur = convolve_image(im, f, 1)
# save_image(blur, "dog-gauss2")

