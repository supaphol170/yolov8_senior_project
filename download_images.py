from simple_image_download import simple_image_download as simp
response = simp.simple_image_download
keyword = ['real face asia people without watermark']
for kw in keyword:
    response().download(kw, 10)