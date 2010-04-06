%% Load Image
% Read BackGround Raw File From Lab Camera
clear all; close all;clc;
fid = fopen('output.raw', 'rb');
bg_matrix = fread(fid, 'uchar');

%% Load Test Insert Images
insert_image = imread('test_insert.png');
%% Transfer Raw File Format (230400x1) To Image File Format (320x240x3)
bg_image = zeros(240,320,3);
for i = 1:240
    for j = 1:320
        for k = 1:3
            index = 1 + 3*(j-1) + (i-1)*320*3 + (k-1);
                bg_image(i,j,k) = (bg_matrix(index,1));
        end
    end
end
%% Apply Forward Transformation
bg_imageHSV = rgb2hsv(bg_image./255);
insert_HSV = rgb2hsv(insert_image./255);

%% Define Background Vectors For Each Colorspace
INSERT_BG_RGB(1,1,1) = insert_image(1,1,1);
INSERT_BG_RGB(1,1,2) = insert_image(1,1,2);
INSERT_BG_RGB(1,1,3) = insert_image(1,1,3);

INSERT_BG_LAB(1,1,1) = insert_HSV(1,1,1);
INSERT_BG_LAB(1,1,2) = insert_HSV(1,1,2);
INSERT_BG_LAB(1,1,3) = insert_HSV(1,1,3);

%% Calculate Mean and Std Dev. of Background HSV Channels
bg_H_array = reshape(bg_imageHSV(:,:,1), 1, 240*320);
bg_S_array = reshape(bg_imageHSV(:,:,2), 1, 240*320);
bg_V_array = reshape(bg_imageHSV(:,:,3), 1, 240*320);

bg_H_array = bg_H_array.*360;
bg_S_array = bg_S_array.*100;
bg_V_array = bg_V_array.*100;

bg_H_mean = mean(bg_H_array);
bg_S_mean = mean(bg_S_array);
bg_V_mean = mean(bg_V_array);
bg_H_std = std(double(bg_H_array));
bg_S_std = std(double(bg_S_array));
bg_V_std = std(double(bg_V_array));

%% Calculate Mean and Std Dev. of Insert LAB Channels
[w,h,d] = size(insert_HSV);
insert_H_array = reshape(insert_HSV(:,:,1), 1, w*h);
insert_H_mean  = mean(insert_H_array);
insert_H_std   = std(double(insert_H_array));
insert_S_array = reshape(insert_HSV(:,:,2), 1, w*h);
insert_S_mean  = mean(insert_S_array);
insert_S_std   = std(double(insert_S_array));
insert_V_array = reshape(insert_HSV(:,:,3), 1, w*h);
insert_V_mean  = mean(insert_V_array);
insert_V_std   = std(double(insert_V_array));

%% Apply blending to insert image
for i=1:w
	for j=1:h
	 if insert_HSV(i,j,1) ~= INSERT_BG_LAB(1,1,1) && insert_HSV(i,j,2) ~= INSERT_BG_LAB(1,1,2) && insert_HSV(i,j,3) ~= INSERT_BG_LAB(1,1,3)
		%insert_HSV(i,j,1) = insert_HSV(i,j,1) - insert_H_mean;
		%insert_HSV(i,j,1) = bg_H_std / insert_H_std * insert_HSV(i,j,1);
		%insert_HSV(i,j,1) = insert_HSV(i,j,1) + bg_H_mean;
		%insert_HSV(i,j,2) = insert_HSV(i,j,2) - insert_S_mean;
		%insert_HSV(i,j,2) = bg_S_std / insert_S_std * insert_HSV(i,j,2);
		%insert_HSV(i,j,2) = insert_HSV(i,j,2) + bg_S_mean;
		insert_HSV(i,j,3) = insert_HSV(i,j,3) - insert_V_mean;
		insert_HSV(i,j,3) = bg_V_std / insert_V_std * insert_HSV(i,j,3);
		insert_HSV(i,j,3) = insert_HSV(i,j,3) + bg_V_mean;
		end
	end
end

%% Invert colorspace transformation
new_bg_rgb = hsv2rgb(bg_imageHSV);
new_insert_rgb = hsv2rgb(insert_HSV);

%% Scale Insert_Images And Put them In The Background Image
%%Definition-----
% originImage: The resized insert_image
% adjustImage: The resized new_insert_rgb, which is adjusted by LAB func
% adjustImage_gaussian: Add Gaussian Noise in adjustImage
% ----------------


originImage = imresize(insert_image,0.75);
adjustImage = imresize(new_insert_rgb, 0.75);
adjustImage_gaussian = imnoise(adjustImage, 'gaussian');

[wi he de] = size(adjustImage);

new_bg_rgb_with_origin = new_bg_rgb;
new_bg_rgb_with_adjust = new_bg_rgb;
new_bg_rgb_with_gussian = new_bg_rgb;


for i = 1:wi
    for j = 1:he
        for k = 1:de
            if (originImage(i,j,k) > 0)
                new_bg_rgb_with_origin(i+140,j+180,k) = originImage(i,j,k);
            end
        end
    end
end

for i = 1:wi
    for j = 1:he
        for k = 1:de
            if (adjustImage(i,j,k) > 100)
                new_bg_rgb_with_adjust(i+140,j+180,k) = adjustImage(i,j,k);
            end
        end
    end
end

for i = 1:wi
    for j = 1:he
        for k = 1:de
            if (adjustImage_gaussian(i,j,k) > 100)
                new_bg_rgb_with_gussian(i+140,j+180,k) = adjustImage_gaussian(i,j,k);
            end
        end
    end
end

%% Display Comparsion
figure; hold on;
subplot(2,2,1); image(new_bg_rgb), title('Background');
subplot(2,2,2); image(new_bg_rgb_with_origin), title('Origina Image');
subplot(2,2,3); image(new_bg_rgb_with_adjust), title('Adjust Image');
subplot(2,2,4); image(new_bg_rgb_with_gussian),title('Gaussian Image');





