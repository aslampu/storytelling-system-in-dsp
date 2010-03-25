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
bg_lab = RGB2Lab(bg_image);
insert_lab = RGB2Lab(insert_image);

%% Define Background Vectors For Each Colorspace
INSERT_BG_RGB(1,1,1) = insert_image(1,1,1);
INSERT_BG_RGB(1,1,2) = insert_image(1,1,2);
INSERT_BG_RGB(1,1,3) = insert_image(1,1,3);

INSERT_BG_LAB(1,1,1) = insert_lab(1,1,1);
INSERT_BG_LAB(1,1,2) = insert_lab(1,1,2);
INSERT_BG_LAB(1,1,3) = insert_lab(1,1,3);

%% Calculate Mean And Std Dev. Of Background LAB Channels 
[w,h,d] = size(bg_lab);
bg_L_array = reshape(bg_lab(:,:,1), 1, w*h);
bg_a_array = reshape(bg_lab(:,:,2), 1, w*h);
bg_b_array = reshape(bg_lab(:,:,3), 1, w*h);

bg_L_mean = mean(bg_L_array);
bg_L_std  = std(double(bg_L_array));
bg_a_mean = mean(bg_a_array);
bg_a_std  = std(double(bg_a_array));
bg_b_mean = mean(bg_b_array);
bg_b_std  = std(double(bg_b_array));

%% Calculate Mean and Std Dev. of Insert LAB Channels
[w,h,d] = size(insert_lab);
insert_L_array = reshape(insert_lab(:,:,1), 1, w*h);
insert_L_mean  = mean(insert_L_array);
insert_L_std   = std(double(insert_L_array));
insert_a_array = reshape(insert_lab(:,:,2), 1, w*h);
insert_a_mean  = mean(insert_a_array);
insert_a_std   = std(double(insert_a_array));
insert_b_array = reshape(insert_lab(:,:,3), 1, w*h);
insert_b_mean  = mean(insert_b_array);
insert_b_std   = std(double(insert_b_array));

%% Apply blending to insert image
for i=1:w
	for j=1:h
		insert_lab(i,j,1) = insert_lab(i,j,1) - insert_L_mean;
		insert_lab(i,j,1) = bg_L_std / insert_L_std * insert_lab(i,j,1);
		insert_lab(i,j,1) = insert_lab(i,j,1) + bg_L_mean;
		insert_lab(i,j,2) = insert_lab(i,j,2) - insert_a_mean;
		insert_lab(i,j,2) = bg_a_std / insert_a_std * insert_lab(i,j,2);
		insert_lab(i,j,2) = insert_lab(i,j,2) + bg_a_mean;
		insert_lab(i,j,3) = insert_lab(i,j,3) - insert_b_mean;
		insert_lab(i,j,3) = bg_b_std / insert_b_std * insert_lab(i,j,3);
		insert_lab(i,j,3) = insert_lab(i,j,3) + bg_b_mean;
	end
end

%% Invert colorspace transformation
new_bg_rgb = Lab2RGB(bg_lab);
new_insert_rgb = Lab2RGB(insert_lab);

%% Scale Insert_Images And Put them In The Background Image
%%Definition-----
% originImage: The resized insert_image
% adjustImage: The resized new_insert_rgb, which is adjusted by LAB func
% adjustImage_gaussian: Add Gaussian Noise in adjustImage
% ----------------


originImage = imresize(insert_image,0.25);
adjustImage = imresize(new_insert_rgb, 0.25);
adjustImage_gaussian = imnoise(adjustImage, 'gaussian');

[wi he de] = size(adjustImage);

new_bg_rgb_with_origin = new_bg_rgb;
new_bg_rgb_with_adjust = new_bg_rgb;
new_bg_rgb_with_gussian = new_bg_rgb;

for i = 1:wi
    for j = 1:he
        for k = 1:de
            if (originImage(i,j,k) > 0)
                new_bg_rgb_with_origin(i+153,j+200,k) = originImage(i,j,k);
            end
        end
    end
end

for i = 1:wi
    for j = 1:he
        for k = 1:de
            if (adjustImage(i,j,k) > 100)
                new_bg_rgb_with_adjust(i+153,j+200,k) = adjustImage(i,j,k);
            end
        end
    end
end

for i = 1:wi
    for j = 1:he
        for k = 1:de
            if (adjustImage_gaussian(i,j,k) > 100)
                new_bg_rgb_with_gussian(i+153,j+200,k) = adjustImage_gaussian(i,j,k);
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





