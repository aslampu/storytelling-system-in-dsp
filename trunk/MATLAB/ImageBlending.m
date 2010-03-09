clear all;
% Load background image
bg_image = imread('test_bg.jpg');

% Load insert image
insert_image = imread('test_insert.jpg');

% Apply forward transformation
bg_lab = RGB2Lab(bg_image);
insert_lab = RGB2Lab(insert_image);

% Define Background Vectors for each colorspace
INSERT_BG_RGB(1,1,1) = insert_image(1,1,1);
INSERT_BG_RGB(1,1,2) = insert_image(1,1,2);
INSERT_BG_RGB(1,1,3) = insert_image(1,1,3);

INSERT_BG_LAB(1,1,1) = insert_lab(1,1,1);
INSERT_BG_LAB(1,1,2) = insert_lab(1,1,2);
INSERT_BG_LAB(1,1,3) = insert_lab(1,1,3);

% Calculate Mean and Std Dev. of Background LAB Channels
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

% Apply blending to insert image
for i=1:w
	for j=1:h
		if insert_lab(i,j,1) ~= INSERT_BG_LAB(1,1,1) && insert_lab(i,j,2) ~= INSERT_BG_LAB(1,1,2) && insert_lab(i,j,3) ~= INSERT_BG_LAB(1,1,3)
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
end

% Superimpose insert image onto background

%%% TO DO!!

% Invert colorspace transformation
new_bg_rgb = Lab2RGB(bg_lab);
new_insert_rgb = Lab2RGB(insert_lab);

% Display results
figure; hold on;
subplot(2,2,1); imagesc(bg_image);
subplot(2,2,2); imagesc(new_bg_rgb);
subplot(2,2,3); imagesc(insert_image);
subplot(2,2,4); imagesc(new_insert_rgb);
