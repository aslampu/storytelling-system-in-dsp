%% Load Image
% Read BackGround Raw File From Lab Camera
clear all; close all;clc;
fid = fopen('output.raw', 'rb');
bg_matrix = fread(fid, 'uchar');

%% Load Test Insert Images
insert_image = imread('test_insert.png');

%insert_image = imread('multi_color_teapot.jpg');
%insert_image = imread('elvis_insert.jpg');
%insert_image = imread('plant_insert.jpg');
insert_image = imread('plant_insert.2.jpg');


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

%bg_image = imread('green_bg.jpg');
%bg_image = imread('outdoor_bg.jpg');

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
bg_L_mean = mean(bg_L_array);
bg_L_std  = std(double(bg_L_array));

bg_a_array = reshape(bg_lab(:,:,2), 1, w*h);
bg_a_mean = mean(bg_a_array);
bg_a_std  = std(double(bg_a_array));

bg_b_array = reshape(bg_lab(:,:,3), 1, w*h);
bg_b_mean = mean(bg_b_array);
bg_b_std  = std(double(bg_b_array));

% Calculate Mean and Std Dev. of Insert LAB Channels excluding background pixels
[w,h,d] = size(insert_lab);
	
insert_L_tmp = 0;
insert_a_tmp = 0;
insert_b_tmp = 0;
count        = 0;

for i=1:w
	for j=1:h
		if insert_lab(i,j,1) ~= INSERT_BG_LAB(1,1,1) && insert_lab(i,j,2) ~= INSERT_BG_LAB(1,1,2) && insert_lab(i,j,3) ~= INSERT_BG_LAB(1,1,3)
			count        = count + 1;
			insert_L_tmp = insert_L_tmp + insert_lab(i,j,1);
			insert_a_tmp = insert_a_tmp + insert_lab(i,j,2);
			insert_b_tmp = insert_b_tmp + insert_lab(i,j,3);
		end
	end
end

insert_L_mean = insert_L_tmp / count;
insert_a_mean = insert_a_tmp / count;
insert_b_mean = insert_b_tmp / count;

insert_L_tmp = 0;
insert_a_tmp = 0;
insert_b_tmp = 0;

for i=1:w
	for j=1:h
		if insert_lab(i,j,1) ~= INSERT_BG_LAB(1,1,1) && insert_lab(i,j,2) ~= INSERT_BG_LAB(1,1,2) && insert_lab(i,j,3) ~= INSERT_BG_LAB(1,1,3)
			insert_L_tmp = insert_L_tmp + (insert_lab(i,j,1) - insert_L_mean)^2;
			insert_a_tmp = insert_a_tmp + (insert_lab(i,j,2) - insert_a_mean)^2;
			insert_b_tmp = insert_b_tmp + (insert_lab(i,j,3) - insert_b_mean)^2;
		end
	end
end

insert_L_std = sqrt(insert_L_tmp / (count - 1));
insert_a_std = sqrt(insert_a_tmp / (count - 1));
insert_b_std = sqrt(insert_b_tmp / (count - 1));

%% Apply partial blending to insert image
s = 1.0;
s_L = 1 - s + s * (bg_L_std / insert_L_std);
s_a = 1 - s + s * (bg_a_std / insert_a_std);
s_b = 1 - s + s * (bg_b_std / insert_b_std);
d = 1.0;
d_L = d * (bg_L_mean - insert_L_mean);
d_a = d * (bg_a_mean - insert_a_mean);
d_b = d * (bg_b_mean - insert_b_mean);

for i=1:w
	for j=1:h
	 if insert_lab(i,j,1) ~= INSERT_BG_LAB(1,1,1) && insert_lab(i,j,2) ~= INSERT_BG_LAB(1,1,2) && insert_lab(i,j,3) ~= INSERT_BG_LAB(1,1,3)
		insert_lab(i,j,1) = s_L * (insert_lab(i,j,1) - insert_L_mean) + insert_L_mean + d_L;
		insert_lab(i,j,2) = s_a * (insert_lab(i,j,2) - insert_a_mean) + insert_a_mean + d_a;
		insert_lab(i,j,3) = s_b * (insert_lab(i,j,3) - insert_b_mean) + insert_b_mean + d_b;
		end
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


originImage = imresize(insert_image,0.5);
adjustImage = imresize(new_insert_rgb, 0.5);
adjustImage_gaussian = imnoise(adjustImage, 'gaussian', 0, 0.005);

[wi he de] = size(adjustImage);

new_bg_rgb_with_origin = new_bg_rgb;
new_bg_rgb_with_adjust = new_bg_rgb;
new_bg_rgb_with_gussian = new_bg_rgb;

i_offset = 20;
j_offset = 100;

for i = 1:wi
    for j = 1:he
        for k = 1:de
            if originImage(i,j,1) ~= INSERT_BG_RGB(1,1,1) && originImage(i,j,2) ~= INSERT_BG_RGB(1,1,2) && originImage(i,j,3) ~= INSERT_BG_RGB(1,1,3)
                new_bg_rgb_with_origin(i + i_offset,j+100,k) = originImage(i,j,k);
                new_bg_rgb_with_adjust(i + i_offset,j+100,k) = adjustImage(i,j,k);
                new_bg_rgb_with_gussian(i + i_offset,j+100,k) = adjustImage_gaussian(i,j,k);
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





