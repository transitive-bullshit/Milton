#include <cstdlib>
#include <cstring>
#include <fstream>
#include <vector>
#include <iostream>

#include <tr1/functional>

#include <obj.hpp>

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

class obj_to_raw_converter
{
public:
  obj_to_raw_converter() {}
  bool convert(std::istream& istream, std::ostream& ostream);
private:
  void info_callback(const std::string& filename, std::size_t line_number, const std::string& message);
  void warning_callback(const std::string& filename, std::size_t line_number, const std::string& message);
  void error_callback(const std::string& filename, std::size_t line_number, const std::string& message);
  void geometric_vertex_callback(obj::float_type x, obj::float_type y, obj::float_type z);
  void texture_vertex_callback(obj::float_type u, obj::float_type v);
  void vertex_normal_callback(obj::float_type x, obj::float_type y, obj::float_type z);
  void triangular_face_geometric_vertices_callback(obj::index_type v1, obj::index_type v2, obj::index_type v3);
  void triangular_face_geometric_vertices_texture_vertices_callback(const obj::index_2_tuple_type& v1_vt1, const obj::index_2_tuple_type& v2_vt2, const obj::index_2_tuple_type& v3_vt3);
  void triangular_face_geometric_vertices_vertex_normals_callback(const obj::index_2_tuple_type& v1_vn1, const obj::index_2_tuple_type& v2_vn2, const obj::index_2_tuple_type& v3_vn3);
  void triangular_face_geometric_vertices_texture_vertices_vertex_normals_callback(const obj::index_3_tuple_type& v1_vt1_vn1, const obj::index_3_tuple_type& v2_vt2_vn2, const obj::index_3_tuple_type& v3_vt3_vn3);
  void quadrilateral_face_geometric_vertices_callback(obj::index_type v1, obj::index_type v2, obj::index_type v3, obj::index_type v4);
  void quadrilateral_face_geometric_vertices_texture_vertices_callback(const obj::index_2_tuple_type& v1_vt1, const obj::index_2_tuple_type& v2_vt2, const obj::index_2_tuple_type& v3_vt3, const obj::index_2_tuple_type& v4_vt4);
  void quadrilateral_face_geometric_vertices_vertex_normals_callback(const obj::index_2_tuple_type& v1_vn1, const obj::index_2_tuple_type& v2_vn2, const obj::index_2_tuple_type& v3_vn3, const obj::index_2_tuple_type& v4_vn4);
  void quadrilateral_face_geometric_vertices_texture_vertices_vertex_normals_callback(const obj::index_3_tuple_type& v1_vt1_vn1, const obj::index_3_tuple_type& v2_vt2_vn2, const obj::index_3_tuple_type& v3_vt3_vn3, const obj::index_3_tuple_type& v4_vt4_vn4);
  void polygonal_face_geometric_vertices_begin_callback(obj::index_type v1, obj::index_type v2, obj::index_type v3);
  void polygonal_face_geometric_vertices_vertex_callback(obj::index_type v);
  void polygonal_face_geometric_vertices_end_callback();
  void polygonal_face_geometric_vertices_texture_vertices_begin_callback(const obj::index_2_tuple_type& v1_vt1, const obj::index_2_tuple_type& v2_vt2, const obj::index_2_tuple_type& v3_vt3);
  void polygonal_face_geometric_vertices_texture_vertices_vertex_callback(const obj::index_2_tuple_type& v_vt);
  void polygonal_face_geometric_vertices_texture_vertices_end_callback();
  void polygonal_face_geometric_vertices_vertex_normals_begin_callback(const obj::index_2_tuple_type& v1_vn1, const obj::index_2_tuple_type& v2_vn2, const obj::index_2_tuple_type& v3_vn3);
  void polygonal_face_geometric_vertices_vertex_normals_vertex_callback(const obj::index_2_tuple_type& v_vn);
  void polygonal_face_geometric_vertices_vertex_normals_end_callback();
  void polygonal_face_geometric_vertices_texture_vertices_vertex_normals_begin_callback(const obj::index_3_tuple_type& v1_vt1_vn1, const obj::index_3_tuple_type& v2_vt2_vn2, const obj::index_3_tuple_type& v3_vt3_vn3);
  void polygonal_face_geometric_vertices_texture_vertices_vertex_normals_vertex_callback(const obj::index_3_tuple_type& v_vt_vn);
  void polygonal_face_geometric_vertices_texture_vertices_vertex_normals_end_callback();
  void group_name_callback(const std::string& group_name);
  void smoothing_group_callback(obj::size_type group_number);
  void object_name_callback(const std::string& object_name);
  void material_library_callback(const std::string& filename);
  void material_name_callback(const std::string& material_name);
  void comment_callback(const std::string& comment);
  std::ostream* ostream_;
  std::vector<std::tr1::tuple<obj::float_type, obj::float_type, obj::float_type> > vertices_;
};

void obj_to_raw_converter::info_callback(const std::string& filename, std::size_t line_number, const std::string& message)
{
  std::cerr << filename << ":" << line_number << ": " << "info: " << message << std::endl;
}

void obj_to_raw_converter::warning_callback(const std::string& filename, std::size_t line_number, const std::string& message)
{
  std::cerr << filename << ":" << line_number << ": " << "warning: " << message << std::endl;
}

void obj_to_raw_converter::error_callback(const std::string& filename, std::size_t line_number, const std::string& message)
{
  std::cerr << filename << ":" << line_number << ": " << "error: " << message << std::endl;
}

void obj_to_raw_converter::geometric_vertex_callback(obj::float_type x, obj::float_type y, obj::float_type z)
{
  vertices_.push_back(std::tr1::tuple<obj::float_type, obj::float_type, obj::float_type >(x, y, z));
}

void obj_to_raw_converter::texture_vertex_callback(obj::float_type u, obj::float_type v)
{
}

void obj_to_raw_converter::vertex_normal_callback(obj::float_type x, obj::float_type y, obj::float_type z)
{
}

void obj_to_raw_converter::triangular_face_geometric_vertices_callback(obj::index_type v1, obj::index_type v2, obj::index_type v3)
{
  (*ostream_) << std::tr1::get<0>(vertices_[v1 - 1])
       << " " << std::tr1::get<1>(vertices_[v1 - 1])
       << " " << std::tr1::get<2>(vertices_[v1 - 1])
       << " " << std::tr1::get<0>(vertices_[v2 - 1])
       << " " << std::tr1::get<1>(vertices_[v2 - 1])
       << " " << std::tr1::get<2>(vertices_[v2 - 1])
       << " " << std::tr1::get<0>(vertices_[v3 - 1])
       << " " << std::tr1::get<1>(vertices_[v3 - 1])
       << " " << std::tr1::get<2>(vertices_[v3 - 1]) << "\n";
}

void obj_to_raw_converter::triangular_face_geometric_vertices_texture_vertices_callback(const obj::index_2_tuple_type& v1_vt1, const obj::index_2_tuple_type& v2_vt2, const obj::index_2_tuple_type& v3_vt3)
{
  (*ostream_) << std::tr1::get<0>(vertices_[std::tr1::get<0>(v1_vt1) - 1])
       << " " << std::tr1::get<1>(vertices_[std::tr1::get<0>(v1_vt1) - 1])
       << " " << std::tr1::get<2>(vertices_[std::tr1::get<0>(v1_vt1) - 1])
       << " " << std::tr1::get<0>(vertices_[std::tr1::get<0>(v2_vt2) - 1])
       << " " << std::tr1::get<1>(vertices_[std::tr1::get<0>(v2_vt2) - 1])
       << " " << std::tr1::get<2>(vertices_[std::tr1::get<0>(v2_vt2) - 1])
       << " " << std::tr1::get<0>(vertices_[std::tr1::get<0>(v3_vt3) - 1])
       << " " << std::tr1::get<1>(vertices_[std::tr1::get<0>(v3_vt3) - 1])
       << " " << std::tr1::get<2>(vertices_[std::tr1::get<0>(v3_vt3) - 1]) << "\n";
}

void obj_to_raw_converter::triangular_face_geometric_vertices_vertex_normals_callback(const obj::index_2_tuple_type& v1_vn1, const obj::index_2_tuple_type& v2_vn2, const obj::index_2_tuple_type& v3_vn3)
{
  (*ostream_) << std::tr1::get<0>(vertices_[std::tr1::get<0>(v1_vn1) - 1])
       << " " << std::tr1::get<1>(vertices_[std::tr1::get<0>(v1_vn1) - 1])
       << " " << std::tr1::get<2>(vertices_[std::tr1::get<0>(v1_vn1) - 1])
       << " " << std::tr1::get<0>(vertices_[std::tr1::get<0>(v2_vn2) - 1])
       << " " << std::tr1::get<1>(vertices_[std::tr1::get<0>(v2_vn2) - 1])
       << " " << std::tr1::get<2>(vertices_[std::tr1::get<0>(v2_vn2) - 1])
       << " " << std::tr1::get<0>(vertices_[std::tr1::get<0>(v3_vn3) - 1])
       << " " << std::tr1::get<1>(vertices_[std::tr1::get<0>(v3_vn3) - 1])
       << " " << std::tr1::get<2>(vertices_[std::tr1::get<0>(v3_vn3) - 1]) << "\n";
}

void obj_to_raw_converter::triangular_face_geometric_vertices_texture_vertices_vertex_normals_callback(const obj::index_3_tuple_type& v1_vt1_vn1, const obj::index_3_tuple_type& v2_vt2_vn2, const obj::index_3_tuple_type& v3_vt3_vn3)
{
  (*ostream_) << std::tr1::get<0>(vertices_[std::tr1::get<0>(v1_vt1_vn1) - 1])
       << " " << std::tr1::get<1>(vertices_[std::tr1::get<0>(v1_vt1_vn1) - 1])
       << " " << std::tr1::get<2>(vertices_[std::tr1::get<0>(v1_vt1_vn1) - 1])
       << " " << std::tr1::get<0>(vertices_[std::tr1::get<0>(v2_vt2_vn2) - 1])
       << " " << std::tr1::get<1>(vertices_[std::tr1::get<0>(v2_vt2_vn2) - 1])
       << " " << std::tr1::get<2>(vertices_[std::tr1::get<0>(v2_vt2_vn2) - 1])
       << " " << std::tr1::get<0>(vertices_[std::tr1::get<0>(v3_vt3_vn3) - 1])
       << " " << std::tr1::get<1>(vertices_[std::tr1::get<0>(v3_vt3_vn3) - 1])
       << " " << std::tr1::get<2>(vertices_[std::tr1::get<0>(v3_vt3_vn3) - 1]) << "\n";
}

void obj_to_raw_converter::quadrilateral_face_geometric_vertices_callback(obj::index_type v1, obj::index_type v2, obj::index_type v3, obj::index_type v4)
{
}

void obj_to_raw_converter::quadrilateral_face_geometric_vertices_texture_vertices_callback(const obj::index_2_tuple_type& v1_vt1, const obj::index_2_tuple_type& v2_vt2, const obj::index_2_tuple_type& v3_vt3, const obj::index_2_tuple_type& v4_vt4)
{
}

void obj_to_raw_converter::quadrilateral_face_geometric_vertices_vertex_normals_callback(const obj::index_2_tuple_type& v1_vn1, const obj::index_2_tuple_type& v2_vn2, const obj::index_2_tuple_type& v3_vn3, const obj::index_2_tuple_type& v4_vn4)
{
}

void obj_to_raw_converter::quadrilateral_face_geometric_vertices_texture_vertices_vertex_normals_callback(const obj::index_3_tuple_type& v1_vt1_vn1, const obj::index_3_tuple_type& v2_vt2_vn2, const obj::index_3_tuple_type& v3_vt3_vn3, const obj::index_3_tuple_type& v4_vt4_vn4)
{
}

void obj_to_raw_converter::polygonal_face_geometric_vertices_begin_callback(obj::index_type v1, obj::index_type v2, obj::index_type v3)
{
}

void obj_to_raw_converter::polygonal_face_geometric_vertices_vertex_callback(obj::index_type v)
{
}

void obj_to_raw_converter::polygonal_face_geometric_vertices_end_callback()
{
}

void obj_to_raw_converter::polygonal_face_geometric_vertices_texture_vertices_begin_callback(const obj::index_2_tuple_type& v1_vt1, const obj::index_2_tuple_type& v2_vt2, const obj::index_2_tuple_type& v3_vt3)
{
}

void obj_to_raw_converter::polygonal_face_geometric_vertices_texture_vertices_vertex_callback(const obj::index_2_tuple_type& v_vt)
{
}

void obj_to_raw_converter::polygonal_face_geometric_vertices_texture_vertices_end_callback()
{
}

void obj_to_raw_converter::polygonal_face_geometric_vertices_vertex_normals_begin_callback(const obj::index_2_tuple_type& v1_vn1, const obj::index_2_tuple_type& v2_vn2, const obj::index_2_tuple_type& v3_vn3)
{
}

void obj_to_raw_converter::polygonal_face_geometric_vertices_vertex_normals_vertex_callback(const obj::index_2_tuple_type& v_vn)
{
}

void obj_to_raw_converter::polygonal_face_geometric_vertices_vertex_normals_end_callback()
{
}

void obj_to_raw_converter::polygonal_face_geometric_vertices_texture_vertices_vertex_normals_begin_callback(const obj::index_3_tuple_type& v1_vt1_vn1, const obj::index_3_tuple_type& v2_vt2_vn2, const obj::index_3_tuple_type& v3_vt3_vn3)
{
}

void obj_to_raw_converter::polygonal_face_geometric_vertices_texture_vertices_vertex_normals_vertex_callback(const obj::index_3_tuple_type& v_vt_vn)
{
}

void obj_to_raw_converter::polygonal_face_geometric_vertices_texture_vertices_vertex_normals_end_callback()
{
}

void obj_to_raw_converter::group_name_callback(const std::string& group_name)
{
}

void obj_to_raw_converter::smoothing_group_callback(obj::size_type group_number)
{
}

void obj_to_raw_converter::object_name_callback(const std::string& object_name)
{
}

void obj_to_raw_converter::material_library_callback(const std::string& filename)
{
}

void obj_to_raw_converter::material_name_callback(const std::string& material_name)
{
}

void obj_to_raw_converter::comment_callback(const std::string& comment)
{
}

bool obj_to_raw_converter::convert(std::istream& istream, std::ostream& ostream)
{
  using namespace std::tr1::placeholders;

  obj::obj_parser::flags_type obj_parser_flags = 0;
  obj_parser_flags |= obj::obj_parser::triangulate_faces;
  obj_parser_flags |= obj::obj_parser::translate_negative_indices;

  obj::obj_parser obj_parser(obj_parser_flags);

  std::string ifilename;

  obj_parser.info_callback(std::tr1::bind(&obj_to_raw_converter::info_callback, this, ifilename, _1, _2));
  obj_parser.warning_callback(std::tr1::bind(&obj_to_raw_converter::warning_callback, this, ifilename, _1, _2));
  obj_parser.error_callback(std::tr1::bind(&obj_to_raw_converter::error_callback, this, ifilename, _1, _2));

  obj_parser.geometric_vertex_callback(std::tr1::bind(&obj_to_raw_converter::geometric_vertex_callback, this, _1, _2, _3));
  obj_parser.texture_vertex_callback(std::tr1::bind(&obj_to_raw_converter::texture_vertex_callback, this, _1, _2));
  obj_parser.vertex_normal_callback(std::tr1::bind(&obj_to_raw_converter::vertex_normal_callback, this, _1, _2, _3));
  obj_parser.face_callbacks(
    std::tr1::bind(&obj_to_raw_converter::triangular_face_geometric_vertices_callback, this, _1, _2, _3),
    std::tr1::bind(&obj_to_raw_converter::triangular_face_geometric_vertices_texture_vertices_callback, this, _1, _2, _3),
    std::tr1::bind(&obj_to_raw_converter::triangular_face_geometric_vertices_vertex_normals_callback, this, _1, _2, _3),
    std::tr1::bind(&obj_to_raw_converter::triangular_face_geometric_vertices_texture_vertices_vertex_normals_callback, this, _1, _2, _3),
    std::tr1::bind(&obj_to_raw_converter::quadrilateral_face_geometric_vertices_callback, this, _1, _2, _3, _4),
    std::tr1::bind(&obj_to_raw_converter::quadrilateral_face_geometric_vertices_texture_vertices_callback, this, _1, _2, _3, _4),
    std::tr1::bind(&obj_to_raw_converter::quadrilateral_face_geometric_vertices_vertex_normals_callback, this, _1, _2, _3, _4),
    std::tr1::bind(&obj_to_raw_converter::quadrilateral_face_geometric_vertices_texture_vertices_vertex_normals_callback, this, _1, _2, _3, _4),
    std::tr1::bind(&obj_to_raw_converter::polygonal_face_geometric_vertices_begin_callback, this, _1, _2, _3),
    std::tr1::bind(&obj_to_raw_converter::polygonal_face_geometric_vertices_vertex_callback, this, _1),
    std::tr1::bind(&obj_to_raw_converter::polygonal_face_geometric_vertices_end_callback, this),
    std::tr1::bind(&obj_to_raw_converter::polygonal_face_geometric_vertices_texture_vertices_begin_callback, this, _1, _2, _3),
    std::tr1::bind(&obj_to_raw_converter::polygonal_face_geometric_vertices_texture_vertices_vertex_callback, this, _1),
    std::tr1::bind(&obj_to_raw_converter::polygonal_face_geometric_vertices_texture_vertices_end_callback, this),
    std::tr1::bind(&obj_to_raw_converter::polygonal_face_geometric_vertices_vertex_normals_begin_callback, this, _1, _2, _3),
    std::tr1::bind(&obj_to_raw_converter::polygonal_face_geometric_vertices_vertex_normals_vertex_callback, this, _1),
    std::tr1::bind(&obj_to_raw_converter::polygonal_face_geometric_vertices_vertex_normals_end_callback, this),
    std::tr1::bind(&obj_to_raw_converter::polygonal_face_geometric_vertices_texture_vertices_vertex_normals_begin_callback, this, _1, _2, _3),
    std::tr1::bind(&obj_to_raw_converter::polygonal_face_geometric_vertices_texture_vertices_vertex_normals_vertex_callback, this, _1),
    std::tr1::bind(&obj_to_raw_converter::polygonal_face_geometric_vertices_texture_vertices_vertex_normals_end_callback, this)
  );
  obj_parser.group_name_callback(std::tr1::bind(&obj_to_raw_converter::group_name_callback, this, _1));
  obj_parser.smoothing_group_callback(std::tr1::bind(&obj_to_raw_converter::smoothing_group_callback, this, _1));
  obj_parser.object_name_callback(std::tr1::bind(&obj_to_raw_converter::object_name_callback, this, _1));
  obj_parser.material_library_callback(std::tr1::bind(&obj_to_raw_converter::material_library_callback, this, _1));
  obj_parser.material_name_callback(std::tr1::bind(&obj_to_raw_converter::material_name_callback, this, _1));
  obj_parser.comment_callback(std::tr1::bind(&obj_to_raw_converter::comment_callback, this, _1));

  ostream_ = &ostream;

  return obj_parser.parse(istream);
}

int main(int argc, char* argv[])
{
  int argi;
  for (argi = 1; argi < argc; ++argi) {

    if (argv[argi][0] != '-') {
      break;
    }
    if (argv[argi][1] == 0) {
      ++argi;
      break;
    }
    char short_opt, *long_opt, *opt_arg;
    if (argv[argi][1] != '-') {
      short_opt = argv[argi][1];
      opt_arg = &argv[argi][2];
      long_opt = &argv[argi][2];
      while (*long_opt != '\0') {
        ++long_opt;
      }
    }
    else {
      short_opt = 0;
      long_opt = &argv[argi][2];
      opt_arg = long_opt;
      while ((*opt_arg != '=') && (*opt_arg != '\0')) {
        ++opt_arg;
      }
      if (*opt_arg == '=') {
        *opt_arg++ = '\0';
      }
    }

    if ((short_opt == 'h') || (std::strcmp(long_opt, "help") == 0)) {
      std::cout << "Usage: obj2raw [OPTION] [[INFILE] OUTFILE]\n";
      std::cout << "Convert from OBJ to POV-Ray RAW triangle format.\n";
      std::cout << "\n";
      std::cout << "  -h, --help       display this help and exit\n";
      std::cout << "  -v, --version    output version information and exit\n";
      std::cout << "\n";
      std::cout << "With no INFILE/OUTFILE, or when INFILE/OUTFILE is -, read standard input/output.\n";
      std::cout << "\n";
      std::cout << "Report bugs to <" << PACKAGE_BUGREPORT << ">.\n";
      return EXIT_SUCCESS;
    }

    else if ((short_opt == 'v') || (std::strcmp(long_opt, "version") == 0)) {
      std::cout << "obj2raw (" << PACKAGE_NAME << ") " << PACKAGE_VERSION << "\n";
      std::cout << "Copyright (C) 2007 " << PACKAGE_AUTHOR << "\n";
      std::cout << "\n";
      std::cout << "This program is free software; you can redistribute it and/or modify\n";
      std::cout << "it under the terms of the GNU General Public License as published by\n";
      std::cout << "the Free Software Foundation; either version 2 of the License, or\n";
      std::cout << "(at your option) any later version.\n";
      std::cout << "\n";
      std::cout << "This program is distributed in the hope that it will be useful,\n";
      std::cout << "but WITHOUT ANY WARRANTY; without even the implied warranty of\n";
      std::cout << "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n";
      std::cout << "GNU General Public License for more details.\n";
      std::cout << "\n";
      std::cout << "You should have received a copy of the GNU General Public License\n";
      std::cout << "along with this program; if not, write to the Free Software\n";
      std::cout << "Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA\n";
      return EXIT_SUCCESS;
    }

    else {
      std::cerr << "obj2raw: " << "invalid option `" << argv[argi] << "'" << "\n";
      std::cerr << "Try `" << argv[0] << " --help' for more information.\n";
      return EXIT_FAILURE;
    }
  }

  int parc = argc - argi;
  char** parv = argv + argi;
  if (parc > 2) {
    std::cerr << "obj2raw: " << "too many parameters" << "\n";
    std::cerr << "Try `" << argv[0] << " --help' for more information.\n";
    return EXIT_FAILURE;
  }

  std::ifstream ifstream;
  const char* ifilename = "";
  if (parc > 0) {
    ifilename = parv[0];
    if (std::strcmp(ifilename, "-") != 0) {
      ifstream.open(ifilename);
      if (!ifstream.is_open()) {
        std::cerr << "obj2raw: " << ifilename << ": " << "no such file or directory" << "\n";
        return EXIT_FAILURE;
      }
    }
  }

  std::ofstream ofstream;
  const char* ofilename = "";
  if (parc > 1) {
    ofilename = parv[1];
    if (std::strcmp(ofilename, "-") != 0) {
      ofstream.open(ofilename);
      if (!ofstream.is_open()) {
        std::cerr << "obj2raw: " << ofilename << ": " << "could not open file" << "\n";
        return EXIT_FAILURE;
      }
    }
  }

  std::istream& istream = ifstream.is_open() ? ifstream : std::cin;
  std::ostream& ostream = ofstream.is_open() ? ofstream : std::cout;

  class obj_to_raw_converter obj_to_raw_converter;
  return obj_to_raw_converter.convert(istream, ostream);
}
