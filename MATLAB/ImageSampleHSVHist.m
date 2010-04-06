sample_blue_1 = imread('cropped.blue.1.jpg');
sample_red_1 = imread('cropped.red.1.jpg');
sample_white_1 = imread('cropped.white.1.jpg');
sample_black_1 = imread('cropped.black.1.jpg');
sample_table_1 = imread('cropped.table.1.jpg');
sample_blue_2 = imread('cropped.blue.2.jpg');
sample_red_2 = imread('cropped.red.2.jpg');
sample_white_2 = imread('cropped.white.2.jpg');
sample_black_2 = imread('cropped.black.2.jpg');
sample_table_2 = imread('cropped.table.2.jpg');
sample_keyboard_1 = imread('cropped.keyboard.1.jpg');
sample_wall_1 = imread('cropped.wall.1.jpg');

blue_hsv_1 = rgb2hsv(sample_blue_1);
red_hsv_1 = rgb2hsv(sample_red_1);
white_hsv_1 = rgb2hsv(sample_white_1);
black_hsv_1 = rgb2hsv(sample_black_1);
table_hsv_1 = rgb2hsv(sample_table_1);
blue_hsv_2 = rgb2hsv(sample_blue_2);
red_hsv_2 = rgb2hsv(sample_red_2);
white_hsv_2 = rgb2hsv(sample_white_2);
black_hsv_2 = rgb2hsv(sample_black_2);
table_hsv_2 = rgb2hsv(sample_table_2);
keyboard_hsv_1 = rgb2hsv(sample_keyboard_1);
wall_hsv_1 = rgb2hsv(sample_wall_1);

figure; hold on;
subplot(2,2,1); image(sample_blue_1); title('Blue Sample');
t1 = blue_hsv_1(:,:,1); t1 = t1(:)'; t2 = blue_hsv_2(:,:,1); t2 = t2(:)';
subplot(2,2,2); hist([t1 t2], 50); title('H Hist');
t1 = blue_hsv_1(:,:,2); t1 = t1(:)'; t2 = blue_hsv_2(:,:,2); t2 = t2(:)';
subplot(2,2,3); hist([t1 t2], 50); title('S Hist');
t1 = blue_hsv_1(:,:,3); t1 = t1(:)'; t2 = blue_hsv_2(:,:,3); t2 = t2(:)';
subplot(2,2,4); hist([t1 t2], 50); title('V Hist');


figure; hold on;
subplot(2,2,1); image(sample_red_1); title('Red Sample');
t1 = red_hsv_1(:,:,1); t1 = t1(:)'; t2 = red_hsv_2(:,:,1); t2 = t2(:)';
subplot(2,2,2); hist([t1 t2], 50); title('H Hist');
t1 = red_hsv_1(:,:,2); t1 = t1(:)'; t2 = red_hsv_2(:,:,2); t2 = t2(:)';
subplot(2,2,3); hist([t1 t2], 50); title('S Hist');
t1 = red_hsv_1(:,:,3); t1 = t1(:)'; t2 = red_hsv_2(:,:,3); t2 = t2(:)';
subplot(2,2,4); hist([t1 t2], 50); title('V Hist');


figure; hold on;
subplot(2,2,1); image(sample_white_2); title('White Sample');
t1 = white_hsv_1(:,:,1); t1 = t1(:)'; t2 = white_hsv_2(:,:,1); t2 = t2(:)';
subplot(2,2,2); hist([t1 t2], 50); title('H Hist');
t1 = white_hsv_1(:,:,2); t1 = t1(:)'; t2 = white_hsv_2(:,:,2); t2 = t2(:)';
subplot(2,2,3); hist([t1 t2], 50); title('S Hist');
t1 = white_hsv_1(:,:,3); t1 = t1(:)'; t2 = white_hsv_2(:,:,3); t2 = t2(:)';
subplot(2,2,4); hist([t1 t2], 50); title('V Hist');


figure; hold on;
subplot(2,2,1); image(sample_black_2); title('Black Sample');
t1 = black_hsv_1(:,:,1); t1 = t1(:)'; t2 = black_hsv_2(:,:,1); t2 = t2(:)';
subplot(2,2,2); hist([t1 t2], 50); title('H Hist');
t1 = black_hsv_1(:,:,2); t1 = t1(:)'; t2 = black_hsv_2(:,:,2); t2 = t2(:)';
subplot(2,2,3); hist([t1 t2], 50); title('S Hist');
t1 = black_hsv_1(:,:,3); t1 = t1(:)'; t2 = black_hsv_2(:,:,3); t2 = t2(:)';
subplot(2,2,4); hist([t1 t2], 50); title('V Hist');


figure; hold on;
subplot(2,2,1); image(sample_table_1); title('Table Sample');
t1 = table_hsv_1(:,:,1); t1 = t1(:)'; t2 = table_hsv_2(:,:,1); t2 = t2(:)';
subplot(2,2,2); hist([t1 t2], 50); title('H Hist');
t1 = table_hsv_1(:,:,2); t1 = t1(:)'; t2 = table_hsv_2(:,:,2); t2 = t2(:)';
subplot(2,2,3); hist([t1 t2], 50); title('S Hist');
t1 = table_hsv_1(:,:,3); t1 = t1(:)'; t2 = table_hsv_2(:,:,3); t2 = t2(:)';
subplot(2,2,4); hist([t1 t2], 50); title('V Hist');


figure; hold on;
subplot(2,2,1); image(sample_keyboard_1); title('Keyboard Sample');
subplot(2,2,2); hist(keyboard_hsv_1(:,:,1), 50); title('H Hist');
subplot(2,2,3); hist(keyboard_hsv_1(:,:,2), 50); title('S Hist');
subplot(2,2,4); hist(keyboard_hsv_1(:,:,3), 50); title('V Hist');


figure; hold on;
subplot(2,2,1); image(sample_wall_1); title('Wall Sample');
subplot(2,2,2); hist(wall_hsv_1(:,:,1), 50); title('H Hist');
subplot(2,2,3); hist(wall_hsv_1(:,:,2), 50); title('S Hist');
subplot(2,2,4); hist(wall_hsv_1(:,:,3), 50); title('V Hist');



