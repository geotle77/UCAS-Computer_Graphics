import imageio

def create_gif_from_images(image_paths, output_gif_path):
    images = []
    for image_path in image_paths:
        images.append(imageio.imread(image_path))
    imageio.mimsave(output_gif_path, images, 'GIF', duration = 0.5)

# 使用示例
image_paths = [f"./outpic/output_{i}.png" for i in range(11)]
output_gif_path = "output.gif"
create_gif_from_images(image_paths, output_gif_path)