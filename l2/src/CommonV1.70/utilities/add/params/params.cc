// params.cc

#include "params.h"
#include <iostream>
#include <cstdlib>

CommonTilton::Image inputImage1;
CommonTilton::Image inputImage2;

namespace CommonTilton
{

 // Constructor
  Params::Params(const string& value)
  {
    version = value;

#ifndef GDAL
    ncols_flag = false;
    nrows_flag = false;
    nbands_flag = false;
    dtype_flag = false;
#endif
    input_image2_flag = false;
    add_value_flag = false;
    copy_color_table_flag = false;

    return;
  }

 // Destructor...
  Params::~Params() 
  {
    return;
  }

 // Print version
  void Params::print_version( )
  {
    cout << endl << version << endl << endl;

    return;
  }

 // Read parameters
  bool Params::read(const char *param_file)
  {
//    cout << "Reading parameters from " << param_file << endl;
    ifstream param_fs(param_file);
    if (!param_fs.is_open())
    {
      cout << "Cannot open input file " << param_file << endl;
      return false;
    }

  // Read initial parameters from parameter file until End-of-File reached
    bool input_image1_flag = false;
    bool output_image_flag = false;
    int sub_pos;
    string line, sub_string;
    while (!param_fs.eof())
    {
      getline(param_fs,line);
      sub_pos = line.find("-");
      while ((!param_fs.eof()) && (sub_pos != 0))
      {
        getline(param_fs,line);
        sub_pos = line.find("-");
      }
      if (line.find("-input_image1") != string::npos)
      {
        input_image1_file = process_line(line,false);
        input_image1_flag = true;
      }
#ifndef GDAL
      if (line.find("-ncols") != string::npos)
      {
        sub_string = process_line(line,false);
        ncols = atoi(sub_string.c_str());
        ncols_flag = true;
      }
      if (line.find("-nrows") != string::npos)
      {
        sub_string = process_line(line,false);
        nrows = atoi(sub_string.c_str());
        nrows_flag = true;
      }
      if (line.find("-nbands") != string::npos)
      {
        sub_string = process_line(line,false);
        nbands = atoi(sub_string.c_str());
        nbands_flag = true;
      }
      if (line.find("-dtype") != string::npos)
      {
        sub_string = process_line(line,false);
        if (sub_string == "UInt8")
        {
          dtype = UInt8;
          dtype_flag = true;
        }
        else if (sub_string == "UInt16")
        {
          dtype = UInt16;
          dtype_flag = true;
        }
        else if (sub_string == "Float32")
        {
          dtype = Float32;
          dtype_flag = true;
        }
        else
        { 
          dtype = Unknown;
          dtype_flag = false;
        }
      }
#endif
      if (line.find("-input_image2") != string::npos)
      {
        input_image2_file = process_line(line,false);
        input_image2_flag = true;
      }
      if (line.find("-add_value") != string::npos)
      {
        sub_string = process_line(line,false);
        add_value = atof(sub_string.c_str());
        add_value_flag = true;
      }
      if (line.find("-output_image") != string::npos)
      {
        output_image_file = process_line(line,false);
        output_image_flag = true;
      }
      if (line.find("-copy_color_table") != string::npos)
      {
        sub_string = process_line(line,false);
        sub_pos = atoi(sub_string.c_str());
        if (sub_pos == 1)
          copy_color_table_flag = true;
        else
          copy_color_table_flag = false;
      }
    }

    param_fs.close();

// Exit with false status if a required parameter is not set, or an improper parameter
// setting is detected.
    if (!input_image1_flag)
    {
      cout << "ERROR: -input_image1 (First input image file) is required" << endl;
      return false;
    }
#ifndef GDAL
    if (!ncols_flag)
    {
      cout << "ERROR: -ncols (Number of columns in input image data) is required" << endl;
      return false;
    }
    if (!nrows_flag)
    {
      cout << "ERROR: -nrows (Number of rows in input image data) is required" << endl;
      return false;
    }
    if (!nbands_flag)
    {
      cout << "ERROR: -nbands (Number of spectral bands in input image data) is required" << endl;
      return false;
    }
    if (!dtype_flag)
    {
      cout << "ERROR: -dtype (Data type of input image data) is required" << endl;
      return false;
    }
#endif
    if (input_image2_flag)
    {
       if (add_value_flag)
         cout << "WARNING: -add_value is ignored because -input_image2 is provided" << endl;
    }
    else
    {
      if (!add_value_flag)
      {
        cout << "ERROR: Either -input_image2 (Second input image file) or" << endl;
        cout << "-add_value (value to add to input_image1) is required" << endl;
        return false;
      }
    }
    if (!output_image_flag)
    {
      cout << "ERROR: -output_image (Output image file) is required" << endl;
      return false;
    }
#ifdef GDAL
    if (!inputImage1.open(input_image1_file))
#else
    if (!inputImage1.open(input_image1_file,ncols,nrows,nbands,dtype))
#endif
    {
      cout << "ERROR: Could not open -input_image1: " << input_image1_file << endl;
      return false;
    }
    if (input_image2_flag)
    {
#ifdef GDAL
      if (!inputImage2.open(input_image2_file))
#else
      if (!inputImage2.open(input_image2_file,ncols,nrows,nbands,dtype))
#endif
      {
        cout << "ERROR: Could not open -input_image2: " << input_image2_file << endl;
        return false;
      }
    }

    return true;
  }

 // Print parameters
  void Params::print()
  {
  // Print version
      cout << "This is " << version << endl << endl;

  // Print input parameters
      cout << "First input image file name: " << input_image1_file << endl;
#ifndef GDAL
      cout << "Input image number of columns = " <<  ncols << endl;
      cout << "Input image number of rows = " <<  nrows << endl;
      cout << "Input image number of bands = " <<  nbands << endl;
      switch (dtype)
      {
        case UInt8:   cout << "Input image data type is UNSIGNED 8-bit (UInt8)" << endl;
                      break;
        case UInt16:  cout << "Input image data type is UNSIGNED 16-bit (UInt16)" << endl;
                      break;
        case Float32: cout << "Input image data type is 32-bit FLOAT (Float32)" << endl;
                      break;
        default:      cout << "WARNING: Input image data type in invalid (Unknown)" << endl;
                      break;
      }
#endif
      if (input_image2_flag)
        cout << "Second input image file name: " << input_image2_file << endl;
      else if (add_value_flag)
        cout << "Add value = " << add_value << endl;

      cout << "Output image file name: " << output_image_file << endl;
      if (copy_color_table_flag)
        cout << "Copying of color table from first input image to output image requested." << endl;

      return;
  }

  string process_line(const string& line, const bool& list_flag)
  {
    int sub_pos, sub_pos_space, sub_pos_tab;
    string sub_string;

    sub_pos_space = line.find_first_of(" ");
    sub_pos_tab = line.find_first_of("\t");
    sub_pos = sub_pos_space;
    if (sub_pos ==  (int) string::npos)
      sub_pos = sub_pos_tab;
    if (sub_pos ==  (int) string::npos)
      return " ";

    sub_string = line.substr(sub_pos);
    while ((sub_string.substr(0,1) == "\t") || (sub_string.substr(0,1) == " "))
      sub_string = line.substr(++sub_pos);
#ifndef WINDOWS
    if (list_flag)
      return sub_string;

    sub_pos = sub_string.find_first_of(" ");
    if (sub_pos !=  (int) string::npos)
      sub_string = sub_string.substr(0,sub_pos);
    sub_pos = sub_string.find_first_of("\t");
    if (sub_pos !=  (int) string::npos)
      sub_string = sub_string.substr(0,sub_pos);
    sub_pos = sub_string.find_first_of("\r");
    if (sub_pos !=  (int) string::npos)
      sub_string = sub_string.substr(0,sub_pos);
#endif
    return sub_string;
  }

} // namespace CommonTilton
