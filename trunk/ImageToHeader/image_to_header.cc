#include <cmath>
#include <cstring>
#include <fstream>

int R_BITS = 5;
int G_BITS = 6;
int B_BITS = 5;

struct RGB_pixel {
  int R;
  int G;
  int B;
};

struct HSV_pixel {
  float H;
  float S;
  float V;
};

HSV_pixel rgb_to_hsv(RGB_pixel inputRGB) {
  // RGB are from 0..1, H is from 0..360, SV from 0..1
  double maxRGB = std::max(inputRGB.R, std::max(inputRGB.G, inputRGB.B));
  double minRGB = std::min(inputRGB.R, std::min(inputRGB.G, inputRGB.B));
  
  double delta = maxRGB - minRGB;
  
  HSV_pixel outputHSV;
  outputHSV.V = maxRGB / 255;
  outputHSV.S = 0;
  outputHSV.H = 0;
  
  if (delta == 0) {
    outputHSV.H = 0;
    outputHSV.S = 0;
  } else {
    outputHSV.S = delta / maxRGB;
    double dR = 60 * (maxRGB - inputRGB.R) / delta + 180;
    double dG = 60 * (maxRGB - inputRGB.G) / delta + 180;
    double dB = 60 * (maxRGB - inputRGB.B) / delta + 180;
    if (inputRGB.R == maxRGB)
      outputHSV.H = dB - dG;
    else if (inputRGB.G == maxRGB)
      outputHSV.H = 120 + dR - dB;
    else
      outputHSV.H = 240 + dG - dR;
  }

  if (outputHSV.H < 0)
    outputHSV.H += 360;
  if (outputHSV.H >= 360)
    outputHSV.H -= 360;

  return outputHSV;
}

static const char *program_name;  // Program name for printed messages
static const char *input_file_path;
static const char *output_file_path;
static const int width = 200;
static const int height = 200;
static bool HSV = false;          // Toggle if output should be HSV
static bool DEBUG = false;        // Toggle debug messages
static float HSV_container[height][width][3];

// The usage() function displays supported execution arguments and options.
void usage() {
  fprintf(stderr, "usage: %s [options] /path/to/input/file.raw /path/to/output/file.h\n", \
          program_name);
  fprintf(stderr, "Options:\n");
  fprintf(stderr, "  -HSV      Use HSV color output\n");
  fprintf(stderr, "  -debug    Print out debug information\n");
  fprintf(stderr, "\n");
  exit(1);
}

// The main function is reponsible for parsing the input arguments, options
// before passing off the data to the functions that generates the header.
int main(int argc, const char* argv[]) {
  program_name = argv[0];

  if (argc < 3)
    usage();

  // Parse input arguments and options
  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];

    if (arg == "-HSV") {
      HSV = true;
      continue;
    }

    if (arg == "-debug") {
      DEBUG = true;
      continue;
    }

    if (i == argc - 2) {
      input_file_path = argv[i];
      continue;
    }

    if (i == argc - 1) {
      output_file_path = argv[i];
      continue;
    }

    usage();
  }

  // Initialize input and output files
  FILE *inputFile = fopen(input_file_path, "rb");
  FILE *outputFile = fopen(output_file_path, "wb");

  if (inputFile == NULL) {
    fprintf(stderr, "Bad input file path\n");
    exit(1);
  }

  if (outputFile == NULL) {
    fprintf(stderr, "Bad output file path\n");
    exit(1);
  }

  // Begin reading pixel data
  int r_value = fgetc(inputFile);
  int g_value = fgetc(inputFile);
  int b_value = fgetc(inputFile);

  // Convert 24-bit pixel to 16-bit pixel (RGB: 5,6,5)
  int rScaled, gScaled, bScaled;

  if (HSV == true) {
    fprintf(outputFile, "float image[%d][%d][3] = {\n", height, width);
  } else {
    fprintf(outputFile, "short image[%d][%d] = {\n", height, width);
  }

  int count_width = 0;
  int count_height = 0;
  while (r_value != EOF && g_value != EOF && b_value != EOF) {
    if (HSV == true) {
      RGB_pixel current_RGB_pixel = {r_value, g_value, b_value};
      HSV_pixel current_HSV_pixel = rgb_to_hsv(current_RGB_pixel);
      HSV_container[count_height][count_width][0] = current_HSV_pixel.H;
      HSV_container[count_height][count_width][1] = current_HSV_pixel.S;
      HSV_container[count_height][count_width][2] = current_HSV_pixel.V;
    } else {
      // Convert 24-bit pixel to 16-bit pixel (RGB: 5,6,5)
      rScaled = (r_value / 255.0) * (pow(2.0, (double)R_BITS) - 1);
      gScaled = (g_value / 255.0) * (pow(2.0, (double)G_BITS) - 1);
      bScaled = (b_value / 255.0) * (pow(2.0, (double)B_BITS) - 1);
  
      // Generate single 16-bit int representation
      int output_value  = rScaled<<(G_BITS+B_BITS);
      output_value += gScaled<<(B_BITS);
      output_value += bScaled;
  
      // Output to file
      if (count_width == 0) {
        fprintf(outputFile, "{0x%04x", output_value);
      } else {
        fprintf(outputFile, ",0x%04x", output_value);
      }

      //fprintf(stderr, "rOrig:   %d gOrig:   %d, bOrig:   %d\n", r_value, g_value, b_value);
      //fprintf(stderr, "rScaled: %d gScaled: %d, bScaled: %d\n", rScaled, gScaled, bScaled);
      //printf("0x%04x\n", output_value);
    }

    count_width++;

    if (count_width == width) {
      count_width = 0;
      count_height++;
      
      if (HSV == false) {
        if (count_height == height)
          fprintf(outputFile, "}\n");
        else
          fprintf(outputFile, "},\n");
      }
    }
    
    r_value = fgetc(inputFile);
    g_value = fgetc(inputFile);
    b_value = fgetc(inputFile);
  }

  if (HSV == true) {
    for (int channel = 0; channel < 3; channel++) {
      fprintf(outputFile, "{\n");
      for (int i = 0; i < height; i++) {
        fprintf(outputFile, "{");
        for (int j = 0; j < width; j++) {
          fprintf(outputFile, "%f, ", HSV_container[i][j][channel]);
        }
        fprintf(outputFile, "},\n");
      }
      fprintf(outputFile, "},\n");
    }
    //fprintf(outputFile, "}\n");
  }

  fprintf(outputFile, "};\n");
  
  // Clean-up
  fclose(inputFile);
  fclose(outputFile);
  
    return 0;
}
