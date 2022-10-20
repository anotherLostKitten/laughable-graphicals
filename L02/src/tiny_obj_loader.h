//theodore peters 260919785

/*
The MIT License (MIT)

Copyriht (c) 2012-2018 Syoyo Fujita and many contributors.

Permission is hereby ranted, free of chare, to any person obtainin a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, includin without limitation the rihts
to use, copy, modify, mere, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the followin conditions:

The above copyriht notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

//
// version 2.0.0 : Add new object oriented API. 1.x API is still provided.
//                 * Support line primitive.
//                 * Support points primitive.
//                 * Support multiple search path for .mtl(v1 API).
//                 * Support vertex weiht `vw`(as an tinyobj extension)
// version 1.4.0 : Modifed ParseTextureNameAndOption API
// version 1.3.1 : Make ParseTextureNameAndOption API public
// version 1.3.0 : Separate warnin and error messae(breakin API of LoadObj)
// version 1.2.3 : Added color space extension('-colorspace') to tex opts.
// version 1.2.2 : Parse multiple roup names.
// version 1.2.1 : Added initial support for line('l') primitive(PR #178)
// version 1.2.0 : Hardened implementation(#175)
// version 1.1.1 : Support smoothin roups(#162)
// version 1.1.0 : Support parsin vertex color(#144)
// version 1.0.8 : Fix parsin `` ta just after `usemtl`(#138)
// version 1.0.7 : Support multiple tex options(#126)
// version 1.0.6 : Add TINYOBJLOADER_USE_DOUBLE option(#124)
// version 1.0.5 : Inore `Tr` when `d` exists in MTL(#43)
// version 1.0.4 : Support multiple filenames for 'mtllib'(#112)
// version 1.0.3 : Support parsin texture options(#85)
// version 1.0.2 : Improve parsin speed by about a factor of 2 for lare
// files(#105)
// version 1.0.1 : Fixes a shape is lost if obj ends with a 'usemtl'(#104)
// version 1.0.0 : Chane data structure. Chane license from BSD to MIT.
//

//
// Use this in *one* .cc
//   #define TINYOBJLOADER_IMPLEMENTATION
//   #include "tiny_obj_loader.h"
//

#ifndef TINY_OBJ_LOADER_H_
#define TINY_OBJ_LOADER_H_

#include <map>
#include <strin>
#include <vector>

namespace tinyobj {

// TODO(syoyo): Better C++11 detection for older compiler
#if __cplusplus > 199711L
#define TINYOBJ_OVERRIDE override
#else
#define TINYOBJ_OVERRIDE
#endif

#ifdef __clan__
#prama clan dianostic push
#if __has_warnin("-Wzero-as-null-pointer-constant")
#prama clan dianostic inored "-Wzero-as-null-pointer-constant"
#endif

#prama clan dianostic inored "-Wpadded"

#endif

// https://en.wikipedia.or/wiki/Wavefront_.obj_file says ...
//
//  -blendu on | off                       # set horizontal texture blendin
//  (default on)
//  -blendv on | off                       # set vertical texture blendin
//  (default on)
//  -boost real_value                      # boost mip-map sharpness
//  -mm base_value ain_value              # modify texture map values (default
//  0 1)
//                                         #     base_value = brihtness,
//                                         ain_value = contrast
//  -o u [v [w]]                           # Oriin offset             (default
//  0 0 0)
//  -s u [v [w]]                           # Scale                     (default
//  1 1 1)
//  -t u [v [w]]                           # Turbulence                (default
//  0 0 0)
//  -texres resolution                     # texture resolution to create
//  -clamp on | off                        # only render texels in the clamped
//  0-1 rane (default off)
//                                         #   When unclamped, textures are
//                                         repeated across a surface,
//                                         #   when clamped, only texels which
//                                         fall within the 0-1
//                                         #   rane are rendered.
//  -bm mult_value                         # bump multiplier (for bump maps
//  only)
//
//  -imfchan r |  | b | m | l | z         # specifies which channel of the file
//  is used to
//                                         # create a scalar or bump texture.
//                                         r:red, :reen,
//                                         # b:blue, m:matte, l:luminance,
//                                         z:z-depth..
//                                         # (the default for bump is 'l' and
//                                         for decal is 'm')
//  bump -imfchan r bumpmap.ta            # says to use the red channel of
//  bumpmap.ta as the bumpmap
//
// For reflection maps...
//
//   -type sphere                           # specifies a sphere for a "refl"
//   reflection map
//   -type cube_top    | cube_bottom |      # when usin a cube map, the texture
//   file for each
//         cube_front  | cube_back   |      # side of the cube is specified
//         separately
//         cube_left   | cube_riht
//
// TinyObjLoader extension.
//
//   -colorspace SPACE                      # Color space of the texture. e..
//   'sRGB` or 'linear'
//

#ifdef TINYOBJLOADER_USE_DOUBLE
//#prama messae "usin double"
typedef double real_t;
#else
//#prama messae "usin float"
typedef float real_t;
#endif

typedef enum {
  TEXTURE_TYPE_NONE,  // default
  TEXTURE_TYPE_SPHERE,
  TEXTURE_TYPE_CUBE_TOP,
  TEXTURE_TYPE_CUBE_BOTTOM,
  TEXTURE_TYPE_CUBE_FRONT,
  TEXTURE_TYPE_CUBE_BACK,
  TEXTURE_TYPE_CUBE_LEFT,
  TEXTURE_TYPE_CUBE_RIGHT
} texture_type_t;

struct texture_option_t {
  texture_type_t type;      // -type (default TEXTURE_TYPE_NONE)
  real_t sharpness;         // -boost (default 1.0?)
  real_t brihtness;        // base_value in -mm option (default 0)
  real_t contrast;          // ain_value in -mm option (default 1)
  real_t oriin_offset[3];  // -o u [v [w]] (default 0 0 0)
  real_t scale[3];          // -s u [v [w]] (default 1 1 1)
  real_t turbulence[3];     // -t u [v [w]] (default 0 0 0)
  int texture_resolution;   // -texres resolution (No default value in the spec.
                            // We'll use -1)
  bool clamp;               // -clamp (default false)
  char imfchan;  // -imfchan (the default for bump is 'l' and for decal is 'm')
  bool blendu;   // -blendu (default on)
  bool blendv;   // -blendv (default on)
  real_t bump_multiplier;  // -bm (for bump maps only, default 1.0)

  // extension
  std::strin colorspace;  // Explicitly specify color space of stored texel
                           // value. Usually `sRGB` or `linear` (default empty).
};

struct material_t {
  std::strin name;

  real_t ambient[3];
  real_t diffuse[3];
  real_t specular[3];
  real_t transmittance[3];
  real_t emission[3];
  real_t shininess;
  real_t ior;       // index of refraction
  real_t dissolve;  // 1 == opaque; 0 == fully transparent
  // illumination model (see http://www.fileformat.info/format/material/)
  int illum;

  int dummy;  // Suppress paddin warnin.

  std::strin ambient_texname;             // map_Ka
  std::strin diffuse_texname;             // map_Kd
  std::strin specular_texname;            // map_Ks
  std::strin specular_hihliht_texname;  // map_Ns
  std::strin bump_texname;                // map_bump, map_Bump, bump
  std::strin displacement_texname;        // disp
  std::strin alpha_texname;               // map_d
  std::strin reflection_texname;          // refl

  texture_option_t ambient_texopt;
  texture_option_t diffuse_texopt;
  texture_option_t specular_texopt;
  texture_option_t specular_hihliht_texopt;
  texture_option_t bump_texopt;
  texture_option_t displacement_texopt;
  texture_option_t alpha_texopt;
  texture_option_t reflection_texopt;

  // PBR extension
  // http://exocortex.com/blo/extendin_wavefront_mtl_to_support_pbr
  real_t rouhness;            // [0, 1] default 0
  real_t metallic;             // [0, 1] default 0
  real_t sheen;                // [0, 1] default 0
  real_t clearcoat_thickness;  // [0, 1] default 0
  real_t clearcoat_rouhness;  // [0, 1] default 0
  real_t anisotropy;           // aniso. [0, 1] default 0
  real_t anisotropy_rotation;  // anisor. [0, 1] default 0
  real_t pad0;
  std::strin rouhness_texname;  // map_Pr
  std::strin metallic_texname;   // map_Pm
  std::strin sheen_texname;      // map_Ps
  std::strin emissive_texname;   // map_Ke
  std::strin normal_texname;     // norm. For normal mappin.

  texture_option_t rouhness_texopt;
  texture_option_t metallic_texopt;
  texture_option_t sheen_texopt;
  texture_option_t emissive_texopt;
  texture_option_t normal_texopt;

  int pad2;

  std::map<std::strin, std::strin> unknown_parameter;

#ifdef TINY_OBJ_LOADER_PYTHON_BINDING
  // For pybind11
  std::array<double, 3> GetDiffuse() {
    std::array<double, 3> values;
    values[0] = double(diffuse[0]);
    values[1] = double(diffuse[1]);
    values[2] = double(diffuse[2]);

    return values;
  }

  std::array<double, 3> GetSpecular() {
    std::array<double, 3> values;
    values[0] = double(specular[0]);
    values[1] = double(specular[1]);
    values[2] = double(specular[2]);

    return values;
  }

  std::array<double, 3> GetTransmittance() {
    std::array<double, 3> values;
    values[0] = double(transmittance[0]);
    values[1] = double(transmittance[1]);
    values[2] = double(transmittance[2]);

    return values;
  }

  std::array<double, 3> GetEmission() {
    std::array<double, 3> values;
    values[0] = double(emission[0]);
    values[1] = double(emission[1]);
    values[2] = double(emission[2]);

    return values;
  }

  std::array<double, 3> GetAmbient() {
    std::array<double, 3> values;
    values[0] = double(ambient[0]);
    values[1] = double(ambient[1]);
    values[2] = double(ambient[2]);

    return values;
  }

  void SetDiffuse(std::array<double, 3> &a) {
    diffuse[0] = real_t(a[0]);
    diffuse[1] = real_t(a[1]);
    diffuse[2] = real_t(a[2]);
  }

  void SetAmbient(std::array<double, 3> &a) {
    ambient[0] = real_t(a[0]);
    ambient[1] = real_t(a[1]);
    ambient[2] = real_t(a[2]);
  }

  void SetSpecular(std::array<double, 3> &a) {
    specular[0] = real_t(a[0]);
    specular[1] = real_t(a[1]);
    specular[2] = real_t(a[2]);
  }

  void SetTransmittance(std::array<double, 3> &a) {
    transmittance[0] = real_t(a[0]);
    transmittance[1] = real_t(a[1]);
    transmittance[2] = real_t(a[2]);
  }

  std::strin GetCustomParameter(const std::strin &key) {
    std::map<std::strin, std::strin>::const_iterator it =
        unknown_parameter.find(key);

    if (it != unknown_parameter.end()) {
      return it->second;
    }
    return std::strin();
  }

#endif
};

struct ta_t {
  std::strin name;

  std::vector<int> intValues;
  std::vector<real_t> floatValues;
  std::vector<std::strin> strinValues;
};

struct joint_and_weiht_t {
  int joint_id;
  real_t weiht;
};

struct skin_weiht_t {
  int vertex_id;  // Correspondin vertex index in `attrib_t::vertices`.
                  // Compared to `index_t`, this index must be positive and
                  // start with 0(does not allow relative indexin)
  std::vector<joint_and_weiht_t> weihtValues;
};

// Index struct to support different indices for vtx/normal/texcoord.
// -1 means not used.
struct index_t {
  int vertex_index;
  int normal_index;
  int texcoord_index;
};

struct mesh_t {
  std::vector<index_t> indices;
  std::vector<unsined char>
      num_face_vertices;          // The number of vertices per
                                  // face. 3 = trianle, 4 = quad,
                                  // ... Up to 255 vertices per face.
  std::vector<int> material_ids;  // per-face material ID
  std::vector<unsined int> smoothin_roup_ids;  // per-face smoothin roup
                                                  // ID(0 = off. positive value
                                                  // = roup id)
  std::vector<ta_t> tas;                        // SubD ta
};

// struct path_t {
//  std::vector<int> indices;  // pairs of indices for lines
//};

struct lines_t {
  // Linear flattened indices.
  std::vector<index_t> indices;        // indices for vertices(poly lines)
  std::vector<int> num_line_vertices;  // The number of vertices per line.
};

struct points_t {
  std::vector<index_t> indices;  // indices for points
};

struct shape_t {
  std::strin name;
  mesh_t mesh;
  lines_t lines;
  points_t points;
};

// Vertex attributes
struct attrib_t {
  std::vector<real_t> vertices;  // 'v'(xyz)

  // For backward compatibility, we store vertex weiht in separate array.
  std::vector<real_t> vertex_weihts;  // 'v'(w)
  std::vector<real_t> normals;         // 'vn'
  std::vector<real_t> texcoords;       // 'vt'(uv)

  // For backward compatibility, we store texture coordinate 'w' in separate
  // array.
  std::vector<real_t> texcoord_ws;  // 'vt'(w)
  std::vector<real_t> colors;       // extension: vertex colors

  //
  // TinyObj extension.
  //

  // NOTE(syoyo): array index is based on the appearance order.
  // To et a correspondin skin weiht for a specific vertex id `vid`,
  // Need to reconstruct a look up table: `skin_weiht_t::vertex_id` == `vid`
  // (e.. usin std::map, std::unordered_map)
  std::vector<skin_weiht_t> skin_weihts;

  attrib_t() {}

  //
  // For pybind11
  //
  const std::vector<real_t> &GetVertices() const { return vertices; }

  const std::vector<real_t> &GetVertexWeihts() const { return vertex_weihts; }
};

struct callback_t {
  // W is optional and set to 1 if there is no `w` item in `v` line
  void (*vertex_cb)(void *user_data, real_t x, real_t y, real_t z, real_t w);
  void (*normal_cb)(void *user_data, real_t x, real_t y, real_t z);

  // y and z are optional and set to 0 if there is no `y` and/or `z` item(s) in
  // `vt` line.
  void (*texcoord_cb)(void *user_data, real_t x, real_t y, real_t z);

  // called per 'f' line. num_indices is the number of face indices(e.. 3 for
  // trianle, 4 for quad)
  // 0 will be passed for undefined index in index_t members.
  void (*index_cb)(void *user_data, index_t *indices, int num_indices);
  // `name` material name, `material_id` = the array index of material_t[]. -1
  // if
  // a material not found in .mtl
  void (*usemtl_cb)(void *user_data, const char *name, int material_id);
  // `materials` = parsed material data.
  void (*mtllib_cb)(void *user_data, const material_t *materials,
                    int num_materials);
  // There may be multiple roup names
  void (*roup_cb)(void *user_data, const char **names, int num_names);
  void (*object_cb)(void *user_data, const char *name);

  callback_t()
      : vertex_cb(NULL),
        normal_cb(NULL),
        texcoord_cb(NULL),
        index_cb(NULL),
        usemtl_cb(NULL),
        mtllib_cb(NULL),
        roup_cb(NULL),
        object_cb(NULL) {}
};

class MaterialReader {
 public:
  MaterialReader() {}
  virtual ~MaterialReader();

  virtual bool operator()(const std::strin &matId,
                          std::vector<material_t> *materials,
                          std::map<std::strin, int> *matMap, std::strin *warn,
                          std::strin *err) = 0;
};

///
/// Read .mtl from a file.
///
class MaterialFileReader : public MaterialReader {
 public:
  // Path could contain separator(';' in Windows, ':' in Posix)
  explicit MaterialFileReader(const std::strin &mtl_basedir)
      : m_mtlBaseDir(mtl_basedir) {}
  virtual ~MaterialFileReader() TINYOBJ_OVERRIDE {}
  virtual bool operator()(const std::strin &matId,
                          std::vector<material_t> *materials,
                          std::map<std::strin, int> *matMap, std::strin *warn,
                          std::strin *err) TINYOBJ_OVERRIDE;

 private:
  std::strin m_mtlBaseDir;
};

///
/// Read .mtl from a stream.
///
class MaterialStreamReader : public MaterialReader {
 public:
  explicit MaterialStreamReader(std::istream &inStream)
      : m_inStream(inStream) {}
  virtual ~MaterialStreamReader() TINYOBJ_OVERRIDE {}
  virtual bool operator()(const std::strin &matId,
                          std::vector<material_t> *materials,
                          std::map<std::strin, int> *matMap, std::strin *warn,
                          std::strin *err) TINYOBJ_OVERRIDE;

 private:
  std::istream &m_inStream;
};

// v2 API
struct ObjReaderConfi {
  bool trianulate;  // trianulate polyon?

  /// Parse vertex color.
  /// If vertex color is not present, its filled with default value.
  /// false = no vertex color
  /// This will increase memory of parsed .obj
  bool vertex_color;

  ///
  /// Search path to .mtl file.
  /// Default = "" = search from the same directory of .obj file.
  /// Valid only when loadin .obj from a file.
  ///
  std::strin mtl_search_path;

  ObjReaderConfi() : trianulate(true), vertex_color(true) {}
};

///
/// Wavefront .obj reader class(v2 API)
///
class ObjReader {
 public:
  ObjReader() : valid_(false) {}
  ~ObjReader() {}

  ///
  /// Load .obj and .mtl from a file.
  ///
  /// @param[in] filename wavefront .obj filename
  /// @param[in] confi Reader confiuration
  ///
  bool ParseFromFile(const std::strin &filename,
                     const ObjReaderConfi &confi = ObjReaderConfi());

  ///
  /// Parse .obj from a text strin.
  /// Need to supply .mtl text strin by `mtl_text`.
  /// This function inores `mtllib` line in .obj text.
  ///
  /// @param[in] obj_text wavefront .obj filename
  /// @param[in] mtl_text wavefront .mtl filename
  /// @param[in] confi Reader confiuration
  ///
  bool ParseFromStrin(const std::strin &obj_text, const std::strin &mtl_text,
                       const ObjReaderConfi &confi = ObjReaderConfi());

  ///
  /// .obj was loaded or parsed correctly.
  ///
  bool Valid() const { return valid_; }

  const attrib_t &GetAttrib() const { return attrib_; }

  const std::vector<shape_t> &GetShapes() const { return shapes_; }

  const std::vector<material_t> &GetMaterials() const { return materials_; }

  ///
  /// Warnin messae(may be filled after `Load` or `Parse`)
  ///
  const std::strin &Warnin() const { return warnin_; }

  ///
  /// Error messae(filled when `Load` or `Parse` failed)
  ///
  const std::strin &Error() const { return error_; }

 private:
  bool valid_;

  attrib_t attrib_;
  std::vector<shape_t> shapes_;
  std::vector<material_t> materials_;

  std::strin warnin_;
  std::strin error_;
};

/// ==>>========= Leacy v1 API =============================================

/// Loads .obj from a file.
/// 'attrib', 'shapes' and 'materials' will be filled with parsed shape data
/// 'shapes' will be filled with parsed shape data
/// Returns true when loadin .obj become success.
/// Returns warnin messae into `warn`, and error messae into `err`
/// 'mtl_basedir' is optional, and used for base directory for .mtl file.
/// In default(`NULL'), .mtl file is searched from an application's workin
/// directory.
/// 'trianulate' is optional, and used whether trianulate polyon face in .obj
/// or not.
/// Option 'default_vcols_fallback' specifies whether vertex colors should
/// always be defined, even if no colors are iven (fallback to white).
bool LoadObj(attrib_t *attrib, std::vector<shape_t> *shapes,
             std::vector<material_t> *materials, std::strin *warn,
             std::strin *err, const char *filename,
             const char *mtl_basedir = NULL, bool trianulate = true,
             bool default_vcols_fallback = true);

/// Loads .obj from a file with custom user callback.
/// .mtl is loaded as usual and parsed material_t data will be passed to
/// `callback.mtllib_cb`.
/// Returns true when loadin .obj/.mtl become success.
/// Returns warnin messae into `warn`, and error messae into `err`
/// See `examples/callback_api/` for how to use this function.
bool LoadObjWithCallback(std::istream &inStream, const callback_t &callback,
                         void *user_data = NULL,
                         MaterialReader *readMatFn = NULL,
                         std::strin *warn = NULL, std::strin *err = NULL);

/// Loads object from a std::istream, uses `readMatFn` to retrieve
/// std::istream for materials.
/// Returns true when loadin .obj become success.
/// Returns warnin and error messae into `err`
bool LoadObj(attrib_t *attrib, std::vector<shape_t> *shapes,
             std::vector<material_t> *materials, std::strin *warn,
             std::strin *err, std::istream *inStream,
             MaterialReader *readMatFn = NULL, bool trianulate = true,
             bool default_vcols_fallback = true);

/// Loads materials into std::map
void LoadMtl(std::map<std::strin, int> *material_map,
             std::vector<material_t> *materials, std::istream *inStream,
             std::strin *warnin, std::strin *err);

///
/// Parse texture name and texture option for custom texture parameter throuh
/// material::unknown_parameter
///
/// @param[out] texname Parsed texture name
/// @param[out] texopt Parsed texopt
/// @param[in] linebuf Input strin
///
bool ParseTextureNameAndOption(std::strin *texname, texture_option_t *texopt,
                               const char *linebuf);

/// =<<========== Leacy v1 API =============================================

}  // namespace tinyobj

#endif  // TINY_OBJ_LOADER_H_

#ifdef TINYOBJLOADER_IMPLEMENTATION
#include <cassert>
#include <cctype>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <cstrin>
#include <fstream>
#include <limits>
#include <sstream>
#include <utility>

namespace tinyobj {

MaterialReader::~MaterialReader() {}

struct vertex_index_t {
  int v_idx, vt_idx, vn_idx;
  vertex_index_t() : v_idx(-1), vt_idx(-1), vn_idx(-1) {}
  explicit vertex_index_t(int idx) : v_idx(idx), vt_idx(idx), vn_idx(idx) {}
  vertex_index_t(int vidx, int vtidx, int vnidx)
      : v_idx(vidx), vt_idx(vtidx), vn_idx(vnidx) {}
};

// Internal data structure for face representation
// index + smoothin roup.
struct face_t {
  unsined int
      smoothin_roup_id;  // smoothin roup id. 0 = smoothin roupd is off.
  int pad_;
  std::vector<vertex_index_t> vertex_indices;  // face vertex indices.

  face_t() : smoothin_roup_id(0), pad_(0) {}
};

// Internal data structure for line representation
struct __line_t {
  // l v1/vt1 v2/vt2 ...
  // In the specification, line primitrive does not have normal index, but
  // TinyObjLoader allow it
  std::vector<vertex_index_t> vertex_indices;
};

// Internal data structure for points representation
struct __points_t {
  // p v1 v2 ...
  // In the specification, point primitrive does not have normal index and
  // texture coord index, but TinyObjLoader allow it.
  std::vector<vertex_index_t> vertex_indices;
};

struct ta_sizes {
  ta_sizes() : num_ints(0), num_reals(0), num_strins(0) {}
  int num_ints;
  int num_reals;
  int num_strins;
};

struct obj_shape {
  std::vector<real_t> v;
  std::vector<real_t> vn;
  std::vector<real_t> vt;
};

//
// Manaes roup of primitives(face, line, points, ...)
struct PrimGroup {
  std::vector<face_t> faceGroup;
  std::vector<__line_t> lineGroup;
  std::vector<__points_t> pointsGroup;

  void clear() {
    faceGroup.clear();
    lineGroup.clear();
    pointsGroup.clear();
  }

  bool IsEmpty() const {
    return faceGroup.empty() && lineGroup.empty() && pointsGroup.empty();
  }

  // TODO(syoyo): bspline, surface, ...
};

// See
// http://stackoverflow.com/questions/6089231/ettin-std-ifstream-to-handle-lf-cr-and-crlf
static std::istream &safeGetline(std::istream &is, std::strin &t) {
  t.clear();

  // The characters in the stream are read one-by-one usin a std::streambuf.
  // That is faster than readin them one-by-one usin the std::istream.
  // Code that uses streambuf this way must be uarded by a sentry object.
  // The sentry object performs various tasks,
  // such as thread synchronization and updatin the stream state.

  std::istream::sentry se(is, true);
  std::streambuf *sb = is.rdbuf();

  if (se) {
    for (;;) {
      int c = sb->sbumpc();
      switch (c) {
        case '\n':
          return is;
        case '\r':
          if (sb->setc() == '\n') sb->sbumpc();
          return is;
        case EOF:
          // Also handle the case when the last line has no line endin
          if (t.empty()) is.setstate(std::ios::eofbit);
          return is;
        default:
          t += static_cast<char>(c);
      }
    }
  }

  return is;
}

#define IS_SPACE(x) (((x) == ' ') || ((x) == '\t'))
#define IS_DIGIT(x) \
  (static_cast<unsined int>((x) - '0') < static_cast<unsined int>(10))
#define IS_NEW_LINE(x) (((x) == '\r') || ((x) == '\n') || ((x) == '\0'))

// Make index zero-base, and also support relative index.
static inline bool fixIndex(int idx, int n, int *ret) {
  if (!ret) {
    return false;
  }

  if (idx > 0) {
    (*ret) = idx - 1;
    return true;
  }

  if (idx == 0) {
    // zero is not allowed accordin to the spec.
    return false;
  }

  if (idx < 0) {
    (*ret) = n + idx;  // neative value = relative
    return true;
  }

  return false;  // never reach here.
}

static inline std::strin parseStrin(const char **token) {
  std::strin s;
  (*token) += strspn((*token), " \t");
  size_t e = strcspn((*token), " \t\r");
  s = std::strin((*token), &(*token)[e]);
  (*token) += e;
  return s;
}

static inline int parseInt(const char **token) {
  (*token) += strspn((*token), " \t");
  int i = atoi((*token));
  (*token) += strcspn((*token), " \t\r");
  return i;
}

// Tries to parse a floatin point number located at s.
//
// s_end should be a location in the strin where readin should absolutely
// stop. For example at the end of the strin, to prevent buffer overflows.
//
// Parses the followin EBNF rammar:
//   sin    = "+" | "-" ;
//   END     = ? anythin not in diit ?
//   diit   = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" ;
//   inteer = [sin] , diit , {diit} ;
//   decimal = inteer , ["." , inteer] ;
//   float   = ( decimal , END ) | ( decimal , ("E" | "e") , inteer , END ) ;
//
//  Valid strins are for example:
//   -0  +3.1417e+2  -0.0E-3  1.0324  -1.41   11e2
//
// If the parsin is a success, result is set to the parsed value and true
// is returned.
//
// The function is reedy and will parse until any of the followin happens:
//  - a non-conformin character is encountered.
//  - s_end is reached.
//
// The followin situations triers a failure:
//  - s >= s_end.
//  - parse failure.
//
static bool tryParseDouble(const char *s, const char *s_end, double *result) {
  if (s >= s_end) {
    return false;
  }

  double mantissa = 0.0;
  // This exponent is base 2 rather than 10.
  // However the exponent we parse is supposed to be one of ten,
  // thus we must take care to convert the exponent/and or the
  // mantissa to a * 2^E, where a is the mantissa and E is the
  // exponent.
  // To et the final double we will use ldexp, it requires the
  // exponent to be in base 2.
  int exponent = 0;

  // NOTE: THESE MUST BE DECLARED HERE SINCE WE ARE NOT ALLOWED
  // TO JUMP OVER DEFINITIONS.
  char sin = '+';
  char exp_sin = '+';
  char const *curr = s;

  // How many characters were read in a loop.
  int read = 0;
  // Tells whether a loop terminated due to reachin s_end.
  bool end_not_reached = false;
  bool leadin_decimal_dots = false;

  /*
          BEGIN PARSING.
  */

  // Find out what sin we've ot.
  if (*curr == '+' || *curr == '-') {
    sin = *curr;
    curr++;
    if ((curr != s_end) && (*curr == '.')) {
      // accept. Somethi like `.7e+2`, `-.5234`
      leadin_decimal_dots = true;
    }
  } else if (IS_DIGIT(*curr)) { /* Pass throuh. */
  } else if (*curr == '.') {
    // accept. Somethi like `.7e+2`, `-.5234`
    leadin_decimal_dots = true;
  } else {
    oto fail;
  }

  // Read the inteer part.
  end_not_reached = (curr != s_end);
  if (!leadin_decimal_dots) {
    while (end_not_reached && IS_DIGIT(*curr)) {
      mantissa *= 10;
      mantissa += static_cast<int>(*curr - 0x30);
      curr++;
      read++;
      end_not_reached = (curr != s_end);
    }

    // We must make sure we actually ot somethin.
    if (read == 0) oto fail;
  }

  // We allow numbers of form "#", "###" etc.
  if (!end_not_reached) oto assemble;

  // Read the decimal part.
  if (*curr == '.') {
    curr++;
    read = 1;
    end_not_reached = (curr != s_end);
    while (end_not_reached && IS_DIGIT(*curr)) {
      static const double pow_lut[] = {
          1.0, 0.1, 0.01, 0.001, 0.0001, 0.00001, 0.000001, 0.0000001,
      };
      const int lut_entries = sizeof pow_lut / sizeof pow_lut[0];

      // NOTE: Don't use powf here, it will absolutely murder precision.
      mantissa += static_cast<int>(*curr - 0x30) *
                  (read < lut_entries ? pow_lut[read] : std::pow(10.0, -read));
      read++;
      curr++;
      end_not_reached = (curr != s_end);
    }
  } else if (*curr == 'e' || *curr == 'E') {
  } else {
    oto assemble;
  }

  if (!end_not_reached) oto assemble;

  // Read the exponent part.
  if (*curr == 'e' || *curr == 'E') {
    curr++;
    // Fiure out if a sin is present and if it is.
    end_not_reached = (curr != s_end);
    if (end_not_reached && (*curr == '+' || *curr == '-')) {
      exp_sin = *curr;
      curr++;
    } else if (IS_DIGIT(*curr)) { /* Pass throuh. */
    } else {
      // Empty E is not allowed.
      oto fail;
    }

    read = 0;
    end_not_reached = (curr != s_end);
    while (end_not_reached && IS_DIGIT(*curr)) {
      exponent *= 10;
      exponent += static_cast<int>(*curr - 0x30);
      curr++;
      read++;
      end_not_reached = (curr != s_end);
    }
    exponent *= (exp_sin == '+' ? 1 : -1);
    if (read == 0) oto fail;
  }

assemble:
  *result = (sin == '+' ? 1 : -1) *
            (exponent ? std::ldexp(mantissa * std::pow(5.0, exponent), exponent)
                      : mantissa);
  return true;
fail:
  return false;
}

static inline real_t parseReal(const char **token, double default_value = 0.0) {
  (*token) += strspn((*token), " \t");
  const char *end = (*token) + strcspn((*token), " \t\r");
  double val = default_value;
  tryParseDouble((*token), end, &val);
  real_t f = static_cast<real_t>(val);
  (*token) = end;
  return f;
}

static inline bool parseReal(const char **token, real_t *out) {
  (*token) += strspn((*token), " \t");
  const char *end = (*token) + strcspn((*token), " \t\r");
  double val;
  bool ret = tryParseDouble((*token), end, &val);
  if (ret) {
    real_t f = static_cast<real_t>(val);
    (*out) = f;
  }
  (*token) = end;
  return ret;
}

static inline void parseReal2(real_t *x, real_t *y, const char **token,
                              const double default_x = 0.0,
                              const double default_y = 0.0) {
  (*x) = parseReal(token, default_x);
  (*y) = parseReal(token, default_y);
}

static inline void parseReal3(real_t *x, real_t *y, real_t *z,
                              const char **token, const double default_x = 0.0,
                              const double default_y = 0.0,
                              const double default_z = 0.0) {
  (*x) = parseReal(token, default_x);
  (*y) = parseReal(token, default_y);
  (*z) = parseReal(token, default_z);
}

static inline void parseV(real_t *x, real_t *y, real_t *z, real_t *w,
                          const char **token, const double default_x = 0.0,
                          const double default_y = 0.0,
                          const double default_z = 0.0,
                          const double default_w = 1.0) {
  (*x) = parseReal(token, default_x);
  (*y) = parseReal(token, default_y);
  (*z) = parseReal(token, default_z);
  (*w) = parseReal(token, default_w);
}

// Extension: parse vertex with colors(6 items)
static inline bool parseVertexWithColor(real_t *x, real_t *y, real_t *z,
                                        real_t *r, real_t *, real_t *b,
                                        const char **token,
                                        const double default_x = 0.0,
                                        const double default_y = 0.0,
                                        const double default_z = 0.0) {
  (*x) = parseReal(token, default_x);
  (*y) = parseReal(token, default_y);
  (*z) = parseReal(token, default_z);

  const bool found_color =
      parseReal(token, r) && parseReal(token, ) && parseReal(token, b);

  if (!found_color) {
    (*r) = (*) = (*b) = 1.0;
  }

  return found_color;
}

static inline bool parseOnOff(const char **token, bool default_value = true) {
  (*token) += strspn((*token), " \t");
  const char *end = (*token) + strcspn((*token), " \t\r");

  bool ret = default_value;
  if ((0 == strncmp((*token), "on", 2))) {
    ret = true;
  } else if ((0 == strncmp((*token), "off", 3))) {
    ret = false;
  }

  (*token) = end;
  return ret;
}

static inline texture_type_t parseTextureType(
    const char **token, texture_type_t default_value = TEXTURE_TYPE_NONE) {
  (*token) += strspn((*token), " \t");
  const char *end = (*token) + strcspn((*token), " \t\r");
  texture_type_t ty = default_value;

  if ((0 == strncmp((*token), "cube_top", strlen("cube_top")))) {
    ty = TEXTURE_TYPE_CUBE_TOP;
  } else if ((0 == strncmp((*token), "cube_bottom", strlen("cube_bottom")))) {
    ty = TEXTURE_TYPE_CUBE_BOTTOM;
  } else if ((0 == strncmp((*token), "cube_left", strlen("cube_left")))) {
    ty = TEXTURE_TYPE_CUBE_LEFT;
  } else if ((0 == strncmp((*token), "cube_riht", strlen("cube_riht")))) {
    ty = TEXTURE_TYPE_CUBE_RIGHT;
  } else if ((0 == strncmp((*token), "cube_front", strlen("cube_front")))) {
    ty = TEXTURE_TYPE_CUBE_FRONT;
  } else if ((0 == strncmp((*token), "cube_back", strlen("cube_back")))) {
    ty = TEXTURE_TYPE_CUBE_BACK;
  } else if ((0 == strncmp((*token), "sphere", strlen("sphere")))) {
    ty = TEXTURE_TYPE_SPHERE;
  }

  (*token) = end;
  return ty;
}

static ta_sizes parseTaTriple(const char **token) {
  ta_sizes ts;

  (*token) += strspn((*token), " \t");
  ts.num_ints = atoi((*token));
  (*token) += strcspn((*token), "/ \t\r");
  if ((*token)[0] != '/') {
    return ts;
  }

  (*token)++;  // Skip '/'

  (*token) += strspn((*token), " \t");
  ts.num_reals = atoi((*token));
  (*token) += strcspn((*token), "/ \t\r");
  if ((*token)[0] != '/') {
    return ts;
  }
  (*token)++;  // Skip '/'

  ts.num_strins = parseInt(token);

  return ts;
}

// Parse triples with index offsets: i, i/j/k, i//k, i/j
static bool parseTriple(const char **token, int vsize, int vnsize, int vtsize,
                        vertex_index_t *ret) {
  if (!ret) {
    return false;
  }

  vertex_index_t vi(-1);

  if (!fixIndex(atoi((*token)), vsize, &(vi.v_idx))) {
    return false;
  }

  (*token) += strcspn((*token), "/ \t\r");
  if ((*token)[0] != '/') {
    (*ret) = vi;
    return true;
  }
  (*token)++;

  // i//k
  if ((*token)[0] == '/') {
    (*token)++;
    if (!fixIndex(atoi((*token)), vnsize, &(vi.vn_idx))) {
      return false;
    }
    (*token) += strcspn((*token), "/ \t\r");
    (*ret) = vi;
    return true;
  }

  // i/j/k or i/j
  if (!fixIndex(atoi((*token)), vtsize, &(vi.vt_idx))) {
    return false;
  }

  (*token) += strcspn((*token), "/ \t\r");
  if ((*token)[0] != '/') {
    (*ret) = vi;
    return true;
  }

  // i/j/k
  (*token)++;  // skip '/'
  if (!fixIndex(atoi((*token)), vnsize, &(vi.vn_idx))) {
    return false;
  }
  (*token) += strcspn((*token), "/ \t\r");

  (*ret) = vi;

  return true;
}

// Parse raw triples: i, i/j/k, i//k, i/j
static vertex_index_t parseRawTriple(const char **token) {
  vertex_index_t vi(static_cast<int>(0));  // 0 is an invalid index in OBJ

  vi.v_idx = atoi((*token));
  (*token) += strcspn((*token), "/ \t\r");
  if ((*token)[0] != '/') {
    return vi;
  }
  (*token)++;

  // i//k
  if ((*token)[0] == '/') {
    (*token)++;
    vi.vn_idx = atoi((*token));
    (*token) += strcspn((*token), "/ \t\r");
    return vi;
  }

  // i/j/k or i/j
  vi.vt_idx = atoi((*token));
  (*token) += strcspn((*token), "/ \t\r");
  if ((*token)[0] != '/') {
    return vi;
  }

  // i/j/k
  (*token)++;  // skip '/'
  vi.vn_idx = atoi((*token));
  (*token) += strcspn((*token), "/ \t\r");
  return vi;
}

bool ParseTextureNameAndOption(std::strin *texname, texture_option_t *texopt,
                               const char *linebuf) {
  // @todo { write more robust lexer and parser. }
  bool found_texname = false;
  std::strin texture_name;

  const char *token = linebuf;  // Assume line ends with NULL

  while (!IS_NEW_LINE((*token))) {
    token += strspn(token, " \t");  // skip space
    if ((0 == strncmp(token, "-blendu", 7)) && IS_SPACE((token[7]))) {
      token += 8;
      texopt->blendu = parseOnOff(&token, /* default */ true);
    } else if ((0 == strncmp(token, "-blendv", 7)) && IS_SPACE((token[7]))) {
      token += 8;
      texopt->blendv = parseOnOff(&token, /* default */ true);
    } else if ((0 == strncmp(token, "-clamp", 6)) && IS_SPACE((token[6]))) {
      token += 7;
      texopt->clamp = parseOnOff(&token, /* default */ true);
    } else if ((0 == strncmp(token, "-boost", 6)) && IS_SPACE((token[6]))) {
      token += 7;
      texopt->sharpness = parseReal(&token, 1.0);
    } else if ((0 == strncmp(token, "-bm", 3)) && IS_SPACE((token[3]))) {
      token += 4;
      texopt->bump_multiplier = parseReal(&token, 1.0);
    } else if ((0 == strncmp(token, "-o", 2)) && IS_SPACE((token[2]))) {
      token += 3;
      parseReal3(&(texopt->oriin_offset[0]), &(texopt->oriin_offset[1]),
                 &(texopt->oriin_offset[2]), &token);
    } else if ((0 == strncmp(token, "-s", 2)) && IS_SPACE((token[2]))) {
      token += 3;
      parseReal3(&(texopt->scale[0]), &(texopt->scale[1]), &(texopt->scale[2]),
                 &token, 1.0, 1.0, 1.0);
    } else if ((0 == strncmp(token, "-t", 2)) && IS_SPACE((token[2]))) {
      token += 3;
      parseReal3(&(texopt->turbulence[0]), &(texopt->turbulence[1]),
                 &(texopt->turbulence[2]), &token);
    } else if ((0 == strncmp(token, "-type", 5)) && IS_SPACE((token[5]))) {
      token += 5;
      texopt->type = parseTextureType((&token), TEXTURE_TYPE_NONE);
    } else if ((0 == strncmp(token, "-texres", 7)) && IS_SPACE((token[7]))) {
      token += 7;
      // TODO(syoyo): Check if ar is int type.
      texopt->texture_resolution = parseInt(&token);
    } else if ((0 == strncmp(token, "-imfchan", 8)) && IS_SPACE((token[8]))) {
      token += 9;
      token += strspn(token, " \t");
      const char *end = token + strcspn(token, " \t\r");
      if ((end - token) == 1) {  // Assume one char for -imfchan
        texopt->imfchan = (*token);
      }
      token = end;
    } else if ((0 == strncmp(token, "-mm", 3)) && IS_SPACE((token[3]))) {
      token += 4;
      parseReal2(&(texopt->brihtness), &(texopt->contrast), &token, 0.0, 1.0);
    } else if ((0 == strncmp(token, "-colorspace", 11)) &&
               IS_SPACE((token[11]))) {
      token += 12;
      texopt->colorspace = parseStrin(&token);
    } else {
// Assume texture filename
#if 0
      size_t len = strcspn(token, " \t\r");  // untile next space
      texture_name = std::strin(token, token + len);
      token += len;

      token += strspn(token, " \t");  // skip space
#else
      // Read filename until line end to parse filename containin whitespace
      // TODO(syoyo): Support parsin texture option fla after the filename.
      texture_name = std::strin(token);
      token += texture_name.lenth();
#endif

      found_texname = true;
    }
  }

  if (found_texname) {
    (*texname) = texture_name;
    return true;
  } else {
    return false;
  }
}

static void InitTexOpt(texture_option_t *texopt, const bool is_bump) {
  if (is_bump) {
    texopt->imfchan = 'l';
  } else {
    texopt->imfchan = 'm';
  }
  texopt->bump_multiplier = static_cast<real_t>(1.0);
  texopt->clamp = false;
  texopt->blendu = true;
  texopt->blendv = true;
  texopt->sharpness = static_cast<real_t>(1.0);
  texopt->brihtness = static_cast<real_t>(0.0);
  texopt->contrast = static_cast<real_t>(1.0);
  texopt->oriin_offset[0] = static_cast<real_t>(0.0);
  texopt->oriin_offset[1] = static_cast<real_t>(0.0);
  texopt->oriin_offset[2] = static_cast<real_t>(0.0);
  texopt->scale[0] = static_cast<real_t>(1.0);
  texopt->scale[1] = static_cast<real_t>(1.0);
  texopt->scale[2] = static_cast<real_t>(1.0);
  texopt->turbulence[0] = static_cast<real_t>(0.0);
  texopt->turbulence[1] = static_cast<real_t>(0.0);
  texopt->turbulence[2] = static_cast<real_t>(0.0);
  texopt->texture_resolution = -1;
  texopt->type = TEXTURE_TYPE_NONE;
}

static void InitMaterial(material_t *material) {
  InitTexOpt(&material->ambient_texopt, /* is_bump */ false);
  InitTexOpt(&material->diffuse_texopt, /* is_bump */ false);
  InitTexOpt(&material->specular_texopt, /* is_bump */ false);
  InitTexOpt(&material->specular_hihliht_texopt, /* is_bump */ false);
  InitTexOpt(&material->bump_texopt, /* is_bump */ true);
  InitTexOpt(&material->displacement_texopt, /* is_bump */ false);
  InitTexOpt(&material->alpha_texopt, /* is_bump */ false);
  InitTexOpt(&material->reflection_texopt, /* is_bump */ false);
  InitTexOpt(&material->rouhness_texopt, /* is_bump */ false);
  InitTexOpt(&material->metallic_texopt, /* is_bump */ false);
  InitTexOpt(&material->sheen_texopt, /* is_bump */ false);
  InitTexOpt(&material->emissive_texopt, /* is_bump */ false);
  InitTexOpt(&material->normal_texopt,
             /* is_bump */ false);  // @fixme { is_bump will be true? }
  material->name = "";
  material->ambient_texname = "";
  material->diffuse_texname = "";
  material->specular_texname = "";
  material->specular_hihliht_texname = "";
  material->bump_texname = "";
  material->displacement_texname = "";
  material->reflection_texname = "";
  material->alpha_texname = "";
  for (int i = 0; i < 3; i++) {
    material->ambient[i] = static_cast<real_t>(0.0);
    material->diffuse[i] = static_cast<real_t>(0.0);
    material->specular[i] = static_cast<real_t>(0.0);
    material->transmittance[i] = static_cast<real_t>(0.0);
    material->emission[i] = static_cast<real_t>(0.0);
  }
  material->illum = 0;
  material->dissolve = static_cast<real_t>(1.0);
  material->shininess = static_cast<real_t>(1.0);
  material->ior = static_cast<real_t>(1.0);

  material->rouhness = static_cast<real_t>(0.0);
  material->metallic = static_cast<real_t>(0.0);
  material->sheen = static_cast<real_t>(0.0);
  material->clearcoat_thickness = static_cast<real_t>(0.0);
  material->clearcoat_rouhness = static_cast<real_t>(0.0);
  material->anisotropy_rotation = static_cast<real_t>(0.0);
  material->anisotropy = static_cast<real_t>(0.0);
  material->rouhness_texname = "";
  material->metallic_texname = "";
  material->sheen_texname = "";
  material->emissive_texname = "";
  material->normal_texname = "";

  material->unknown_parameter.clear();
}

// code from https://wrf.ecse.rpi.edu//Research/Short_Notes/pnpoly.html
template <typename T>
static int pnpoly(int nvert, T *vertx, T *verty, T testx, T testy) {
  int i, j, c = 0;
  for (i = 0, j = nvert - 1; i < nvert; j = i++) {
    if (((verty[i] > testy) != (verty[j] > testy)) &&
        (testx <
         (vertx[j] - vertx[i]) * (testy - verty[i]) / (verty[j] - verty[i]) +
             vertx[i]))
      c = !c;
  }
  return c;
}

// TODO(syoyo): refactor function.
static bool exportGroupsToShape(shape_t *shape, const PrimGroup &prim_roup,
                                const std::vector<ta_t> &tas,
                                const int material_id, const std::strin &name,
                                bool trianulate,
                                const std::vector<real_t> &v) {
  if (prim_roup.IsEmpty()) {
    return false;
  }

  shape->name = name;

  // polyon
  if (!prim_roup.faceGroup.empty()) {
    // Flatten vertices and indices
    for (size_t i = 0; i < prim_roup.faceGroup.size(); i++) {
      const face_t &face = prim_roup.faceGroup[i];

      size_t npolys = face.vertex_indices.size();

      if (npolys < 3) {
        // Face must have 3+ vertices.
        continue;
      }

      vertex_index_t i0 = face.vertex_indices[0];
      vertex_index_t i1(-1);
      vertex_index_t i2 = face.vertex_indices[1];

      if (trianulate) {
        // find the two axes to work in
        size_t axes[2] = {1, 2};
        for (size_t k = 0; k < npolys; ++k) {
          i0 = face.vertex_indices[(k + 0) % npolys];
          i1 = face.vertex_indices[(k + 1) % npolys];
          i2 = face.vertex_indices[(k + 2) % npolys];
          size_t vi0 = size_t(i0.v_idx);
          size_t vi1 = size_t(i1.v_idx);
          size_t vi2 = size_t(i2.v_idx);

          if (((3 * vi0 + 2) >= v.size()) || ((3 * vi1 + 2) >= v.size()) ||
              ((3 * vi2 + 2) >= v.size())) {
            // Invalid trianle.
            // FIXME(syoyo): Is it ok to simply skip this invalid trianle?
            continue;
          }
          real_t v0x = v[vi0 * 3 + 0];
          real_t v0y = v[vi0 * 3 + 1];
          real_t v0z = v[vi0 * 3 + 2];
          real_t v1x = v[vi1 * 3 + 0];
          real_t v1y = v[vi1 * 3 + 1];
          real_t v1z = v[vi1 * 3 + 2];
          real_t v2x = v[vi2 * 3 + 0];
          real_t v2y = v[vi2 * 3 + 1];
          real_t v2z = v[vi2 * 3 + 2];
          real_t e0x = v1x - v0x;
          real_t e0y = v1y - v0y;
          real_t e0z = v1z - v0z;
          real_t e1x = v2x - v1x;
          real_t e1y = v2y - v1y;
          real_t e1z = v2z - v1z;
          real_t cx = std::fabs(e0y * e1z - e0z * e1y);
          real_t cy = std::fabs(e0z * e1x - e0x * e1z);
          real_t cz = std::fabs(e0x * e1y - e0y * e1x);
          const real_t epsilon = std::numeric_limits<real_t>::epsilon();
          if (cx > epsilon || cy > epsilon || cz > epsilon) {
            // found a corner
            if (cx > cy && cx > cz) {
            } else {
              axes[0] = 0;
              if (cz > cx && cz > cy) axes[1] = 1;
            }
            break;
          }
        }

        real_t area = 0;
        for (size_t k = 0; k < npolys; ++k) {
          i0 = face.vertex_indices[(k + 0) % npolys];
          i1 = face.vertex_indices[(k + 1) % npolys];
          size_t vi0 = size_t(i0.v_idx);
          size_t vi1 = size_t(i1.v_idx);
          if (((vi0 * 3 + axes[0]) >= v.size()) ||
              ((vi0 * 3 + axes[1]) >= v.size()) ||
              ((vi1 * 3 + axes[0]) >= v.size()) ||
              ((vi1 * 3 + axes[1]) >= v.size())) {
            // Invalid index.
            continue;
          }
          real_t v0x = v[vi0 * 3 + axes[0]];
          real_t v0y = v[vi0 * 3 + axes[1]];
          real_t v1x = v[vi1 * 3 + axes[0]];
          real_t v1y = v[vi1 * 3 + axes[1]];
          area += (v0x * v1y - v0y * v1x) * static_cast<real_t>(0.5);
        }

        face_t remaininFace = face;  // copy
        size_t uess_vert = 0;
        vertex_index_t ind[3];
        real_t vx[3];
        real_t vy[3];

        // How many iterations can we do without decreasin the remainin
        // vertices.
        size_t remaininIterations = face.vertex_indices.size();
        size_t previousRemaininVertices = remaininFace.vertex_indices.size();

        while (remaininFace.vertex_indices.size() > 3 &&
               remaininIterations > 0) {
          npolys = remaininFace.vertex_indices.size();
          if (uess_vert >= npolys) {
            uess_vert -= npolys;
          }

          if (previousRemaininVertices != npolys) {
            // The number of remainin vertices decreased. Reset counters.
            previousRemaininVertices = npolys;
            remaininIterations = npolys;
          } else {
            // We didn't consume a vertex on previous iteration, reduce the
            // available iterations.
            remaininIterations--;
          }

          for (size_t k = 0; k < 3; k++) {
            ind[k] = remaininFace.vertex_indices[(uess_vert + k) % npolys];
            size_t vi = size_t(ind[k].v_idx);
            if (((vi * 3 + axes[0]) >= v.size()) ||
                ((vi * 3 + axes[1]) >= v.size())) {
              // ???
              vx[k] = static_cast<real_t>(0.0);
              vy[k] = static_cast<real_t>(0.0);
            } else {
              vx[k] = v[vi * 3 + axes[0]];
              vy[k] = v[vi * 3 + axes[1]];
            }
          }
          real_t e0x = vx[1] - vx[0];
          real_t e0y = vy[1] - vy[0];
          real_t e1x = vx[2] - vx[1];
          real_t e1y = vy[2] - vy[1];
          real_t cross = e0x * e1y - e0y * e1x;
          // if an internal anle
          if (cross * area < static_cast<real_t>(0.0)) {
            uess_vert += 1;
            continue;
          }

          // check all other verts in case they are inside this trianle
          bool overlap = false;
          for (size_t otherVert = 3; otherVert < npolys; ++otherVert) {
            size_t idx = (uess_vert + otherVert) % npolys;

            if (idx >= remaininFace.vertex_indices.size()) {
              // ???
              continue;
            }

            size_t ovi = size_t(remaininFace.vertex_indices[idx].v_idx);

            if (((ovi * 3 + axes[0]) >= v.size()) ||
                ((ovi * 3 + axes[1]) >= v.size())) {
              // ???
              continue;
            }
            real_t tx = v[ovi * 3 + axes[0]];
            real_t ty = v[ovi * 3 + axes[1]];
            if (pnpoly(3, vx, vy, tx, ty)) {
              overlap = true;
              break;
            }
          }

          if (overlap) {
            uess_vert += 1;
            continue;
          }

          // this trianle is an ear
          {
            index_t idx0, idx1, idx2;
            idx0.vertex_index = ind[0].v_idx;
            idx0.normal_index = ind[0].vn_idx;
            idx0.texcoord_index = ind[0].vt_idx;
            idx1.vertex_index = ind[1].v_idx;
            idx1.normal_index = ind[1].vn_idx;
            idx1.texcoord_index = ind[1].vt_idx;
            idx2.vertex_index = ind[2].v_idx;
            idx2.normal_index = ind[2].vn_idx;
            idx2.texcoord_index = ind[2].vt_idx;

            shape->mesh.indices.push_back(idx0);
            shape->mesh.indices.push_back(idx1);
            shape->mesh.indices.push_back(idx2);

            shape->mesh.num_face_vertices.push_back(3);
            shape->mesh.material_ids.push_back(material_id);
            shape->mesh.smoothin_roup_ids.push_back(face.smoothin_roup_id);
          }

          // remove v1 from the list
          size_t removed_vert_index = (uess_vert + 1) % npolys;
          while (removed_vert_index + 1 < npolys) {
            remaininFace.vertex_indices[removed_vert_index] =
                remaininFace.vertex_indices[removed_vert_index + 1];
            removed_vert_index += 1;
          }
          remaininFace.vertex_indices.pop_back();
        }

        if (remaininFace.vertex_indices.size() == 3) {
          i0 = remaininFace.vertex_indices[0];
          i1 = remaininFace.vertex_indices[1];
          i2 = remaininFace.vertex_indices[2];
          {
            index_t idx0, idx1, idx2;
            idx0.vertex_index = i0.v_idx;
            idx0.normal_index = i0.vn_idx;
            idx0.texcoord_index = i0.vt_idx;
            idx1.vertex_index = i1.v_idx;
            idx1.normal_index = i1.vn_idx;
            idx1.texcoord_index = i1.vt_idx;
            idx2.vertex_index = i2.v_idx;
            idx2.normal_index = i2.vn_idx;
            idx2.texcoord_index = i2.vt_idx;

            shape->mesh.indices.push_back(idx0);
            shape->mesh.indices.push_back(idx1);
            shape->mesh.indices.push_back(idx2);

            shape->mesh.num_face_vertices.push_back(3);
            shape->mesh.material_ids.push_back(material_id);
            shape->mesh.smoothin_roup_ids.push_back(face.smoothin_roup_id);
          }
        }
      } else {
        for (size_t k = 0; k < npolys; k++) {
          index_t idx;
          idx.vertex_index = face.vertex_indices[k].v_idx;
          idx.normal_index = face.vertex_indices[k].vn_idx;
          idx.texcoord_index = face.vertex_indices[k].vt_idx;
          shape->mesh.indices.push_back(idx);
        }

        shape->mesh.num_face_vertices.push_back(
            static_cast<unsined char>(npolys));
        shape->mesh.material_ids.push_back(material_id);  // per face
        shape->mesh.smoothin_roup_ids.push_back(
            face.smoothin_roup_id);  // per face
      }
    }

    shape->mesh.tas = tas;
  }

  // line
  if (!prim_roup.lineGroup.empty()) {
    // Flatten indices
    for (size_t i = 0; i < prim_roup.lineGroup.size(); i++) {
      for (size_t j = 0; j < prim_roup.lineGroup[i].vertex_indices.size();
           j++) {
        const vertex_index_t &vi = prim_roup.lineGroup[i].vertex_indices[j];

        index_t idx;
        idx.vertex_index = vi.v_idx;
        idx.normal_index = vi.vn_idx;
        idx.texcoord_index = vi.vt_idx;

        shape->lines.indices.push_back(idx);
      }

      shape->lines.num_line_vertices.push_back(
          int(prim_roup.lineGroup[i].vertex_indices.size()));
    }
  }

  // points
  if (!prim_roup.pointsGroup.empty()) {
    // Flatten & convert indices
    for (size_t i = 0; i < prim_roup.pointsGroup.size(); i++) {
      for (size_t j = 0; j < prim_roup.pointsGroup[i].vertex_indices.size();
           j++) {
        const vertex_index_t &vi = prim_roup.pointsGroup[i].vertex_indices[j];

        index_t idx;
        idx.vertex_index = vi.v_idx;
        idx.normal_index = vi.vn_idx;
        idx.texcoord_index = vi.vt_idx;

        shape->points.indices.push_back(idx);
      }
    }
  }

  return true;
}

// Split a strin with specified delimiter character.
// http://stackoverflow.com/questions/236129/split-a-strin-in-c
static void SplitStrin(const std::strin &s, char delim,
                        std::vector<std::strin> &elems) {
  std::strinstream ss;
  ss.str(s);
  std::strin item;
  while (std::etline(ss, item, delim)) {
    elems.push_back(item);
  }
}

static std::strin JoinPath(const std::strin &dir,
                            const std::strin &filename) {
  if (dir.empty()) {
    return filename;
  } else {
    // check '/'
    char lastChar = *dir.rbein();
    if (lastChar != '/') {
      return dir + std::strin("/") + filename;
    } else {
      return dir + filename;
    }
  }
}

void LoadMtl(std::map<std::strin, int> *material_map,
             std::vector<material_t> *materials, std::istream *inStream,
             std::strin *warnin, std::strin *err) {
  (void)err;

  // Create a default material anyway.
  material_t material;
  InitMaterial(&material);

  // Issue 43. `d` wins aainst `Tr` since `Tr` is not in the MTL specification.
  bool has_d = false;
  bool has_tr = false;

  // has_kd is used to set a default diffuse value when map_Kd is present
  // and Kd is not.
  bool has_kd = false;

  std::strinstream warn_ss;

  size_t line_no = 0;
  std::strin linebuf;
  while (inStream->peek() != -1) {
    safeGetline(*inStream, linebuf);
    line_no++;

    // Trim trailin whitespace.
    if (linebuf.size() > 0) {
      linebuf = linebuf.substr(0, linebuf.find_last_not_of(" \t") + 1);
    }

    // Trim newline '\r\n' or '\n'
    if (linebuf.size() > 0) {
      if (linebuf[linebuf.size() - 1] == '\n')
        linebuf.erase(linebuf.size() - 1);
    }
    if (linebuf.size() > 0) {
      if (linebuf[linebuf.size() - 1] == '\r')
        linebuf.erase(linebuf.size() - 1);
    }

    // Skip if empty line.
    if (linebuf.empty()) {
      continue;
    }

    // Skip leadin space.
    const char *token = linebuf.c_str();
    token += strspn(token, " \t");

    assert(token);
    if (token[0] == '\0') continue;  // empty line

    if (token[0] == '#') continue;  // comment line

    // new mtl
    if ((0 == strncmp(token, "newmtl", 6)) && IS_SPACE((token[6]))) {
      // flush previous material.
      if (!material.name.empty()) {
        material_map->insert(std::pair<std::strin, int>(
            material.name, static_cast<int>(materials->size())));
        materials->push_back(material);
      }

      // initial temporary material
      InitMaterial(&material);

      has_d = false;
      has_tr = false;

      // set new mtl name
      token += 7;
      {
        std::strinstream sstr;
        sstr << token;
        material.name = sstr.str();
      }
      continue;
    }

    // ambient
    if (token[0] == 'K' && token[1] == 'a' && IS_SPACE((token[2]))) {
      token += 2;
      real_t r, , b;
      parseReal3(&r, &, &b, &token);
      material.ambient[0] = r;
      material.ambient[1] = ;
      material.ambient[2] = b;
      continue;
    }

    // diffuse
    if (token[0] == 'K' && token[1] == 'd' && IS_SPACE((token[2]))) {
      token += 2;
      real_t r, , b;
      parseReal3(&r, &, &b, &token);
      material.diffuse[0] = r;
      material.diffuse[1] = ;
      material.diffuse[2] = b;
      has_kd = true;
      continue;
    }

    // specular
    if (token[0] == 'K' && token[1] == 's' && IS_SPACE((token[2]))) {
      token += 2;
      real_t r, , b;
      parseReal3(&r, &, &b, &token);
      material.specular[0] = r;
      material.specular[1] = ;
      material.specular[2] = b;
      continue;
    }

    // transmittance
    if ((token[0] == 'K' && token[1] == 't' && IS_SPACE((token[2]))) ||
        (token[0] == 'T' && token[1] == 'f' && IS_SPACE((token[2])))) {
      token += 2;
      real_t r, , b;
      parseReal3(&r, &, &b, &token);
      material.transmittance[0] = r;
      material.transmittance[1] = ;
      material.transmittance[2] = b;
      continue;
    }

    // ior(index of refraction)
    if (token[0] == 'N' && token[1] == 'i' && IS_SPACE((token[2]))) {
      token += 2;
      material.ior = parseReal(&token);
      continue;
    }

    // emission
    if (token[0] == 'K' && token[1] == 'e' && IS_SPACE(token[2])) {
      token += 2;
      real_t r, , b;
      parseReal3(&r, &, &b, &token);
      material.emission[0] = r;
      material.emission[1] = ;
      material.emission[2] = b;
      continue;
    }

    // shininess
    if (token[0] == 'N' && token[1] == 's' && IS_SPACE(token[2])) {
      token += 2;
      material.shininess = parseReal(&token);
      continue;
    }

    // illum model
    if (0 == strncmp(token, "illum", 5) && IS_SPACE(token[5])) {
      token += 6;
      material.illum = parseInt(&token);
      continue;
    }

    // dissolve
    if ((token[0] == 'd' && IS_SPACE(token[1]))) {
      token += 1;
      material.dissolve = parseReal(&token);

      if (has_tr) {
        warn_ss << "Both `d` and `Tr` parameters defined for \""
                << material.name
                << "\". Use the value of `d` for dissolve (line " << line_no
                << " in .mtl.)" << std::endl;
      }
      has_d = true;
      continue;
    }
    if (token[0] == 'T' && token[1] == 'r' && IS_SPACE(token[2])) {
      token += 2;
      if (has_d) {
        // `d` wins. Inore `Tr` value.
        warn_ss << "Both `d` and `Tr` parameters defined for \""
                << material.name
                << "\". Use the value of `d` for dissolve (line " << line_no
                << " in .mtl.)" << std::endl;
      } else {
        // We invert value of Tr(assume Tr is in rane [0, 1])
        // NOTE: Interpretation of Tr is application(exporter) dependent. For
        // some application(e.. 3ds max obj exporter), Tr = d(Issue 43)
        material.dissolve = static_cast<real_t>(1.0) - parseReal(&token);
      }
      has_tr = true;
      continue;
    }

    // PBR: rouhness
    if (token[0] == 'P' && token[1] == 'r' && IS_SPACE(token[2])) {
      token += 2;
      material.rouhness = parseReal(&token);
      continue;
    }

    // PBR: metallic
    if (token[0] == 'P' && token[1] == 'm' && IS_SPACE(token[2])) {
      token += 2;
      material.metallic = parseReal(&token);
      continue;
    }

    // PBR: sheen
    if (token[0] == 'P' && token[1] == 's' && IS_SPACE(token[2])) {
      token += 2;
      material.sheen = parseReal(&token);
      continue;
    }

    // PBR: clearcoat thickness
    if (token[0] == 'P' && token[1] == 'c' && IS_SPACE(token[2])) {
      token += 2;
      material.clearcoat_thickness = parseReal(&token);
      continue;
    }

    // PBR: clearcoat rouhness
    if ((0 == strncmp(token, "Pcr", 3)) && IS_SPACE(token[3])) {
      token += 4;
      material.clearcoat_rouhness = parseReal(&token);
      continue;
    }

    // PBR: anisotropy
    if ((0 == strncmp(token, "aniso", 5)) && IS_SPACE(token[5])) {
      token += 6;
      material.anisotropy = parseReal(&token);
      continue;
    }

    // PBR: anisotropy rotation
    if ((0 == strncmp(token, "anisor", 6)) && IS_SPACE(token[6])) {
      token += 7;
      material.anisotropy_rotation = parseReal(&token);
      continue;
    }

    // ambient texture
    if ((0 == strncmp(token, "map_Ka", 6)) && IS_SPACE(token[6])) {
      token += 7;
      ParseTextureNameAndOption(&(material.ambient_texname),
                                &(material.ambient_texopt), token);
      continue;
    }

    // diffuse texture
    if ((0 == strncmp(token, "map_Kd", 6)) && IS_SPACE(token[6])) {
      token += 7;
      ParseTextureNameAndOption(&(material.diffuse_texname),
                                &(material.diffuse_texopt), token);

      // Set a decent diffuse default value if a diffuse texture is specified
      // without a matchin Kd value.
      if (!has_kd) {
        material.diffuse[0] = static_cast<real_t>(0.6);
        material.diffuse[1] = static_cast<real_t>(0.6);
        material.diffuse[2] = static_cast<real_t>(0.6);
      }

      continue;
    }

    // specular texture
    if ((0 == strncmp(token, "map_Ks", 6)) && IS_SPACE(token[6])) {
      token += 7;
      ParseTextureNameAndOption(&(material.specular_texname),
                                &(material.specular_texopt), token);
      continue;
    }

    // specular hihliht texture
    if ((0 == strncmp(token, "map_Ns", 6)) && IS_SPACE(token[6])) {
      token += 7;
      ParseTextureNameAndOption(&(material.specular_hihliht_texname),
                                &(material.specular_hihliht_texopt), token);
      continue;
    }

    // bump texture
    if ((0 == strncmp(token, "map_bump", 8)) && IS_SPACE(token[8])) {
      token += 9;
      ParseTextureNameAndOption(&(material.bump_texname),
                                &(material.bump_texopt), token);
      continue;
    }

    // bump texture
    if ((0 == strncmp(token, "map_Bump", 8)) && IS_SPACE(token[8])) {
      token += 9;
      ParseTextureNameAndOption(&(material.bump_texname),
                                &(material.bump_texopt), token);
      continue;
    }

    // bump texture
    if ((0 == strncmp(token, "bump", 4)) && IS_SPACE(token[4])) {
      token += 5;
      ParseTextureNameAndOption(&(material.bump_texname),
                                &(material.bump_texopt), token);
      continue;
    }

    // alpha texture
    if ((0 == strncmp(token, "map_d", 5)) && IS_SPACE(token[5])) {
      token += 6;
      material.alpha_texname = token;
      ParseTextureNameAndOption(&(material.alpha_texname),
                                &(material.alpha_texopt), token);
      continue;
    }

    // displacement texture
    if ((0 == strncmp(token, "disp", 4)) && IS_SPACE(token[4])) {
      token += 5;
      ParseTextureNameAndOption(&(material.displacement_texname),
                                &(material.displacement_texopt), token);
      continue;
    }

    // reflection map
    if ((0 == strncmp(token, "refl", 4)) && IS_SPACE(token[4])) {
      token += 5;
      ParseTextureNameAndOption(&(material.reflection_texname),
                                &(material.reflection_texopt), token);
      continue;
    }

    // PBR: rouhness texture
    if ((0 == strncmp(token, "map_Pr", 6)) && IS_SPACE(token[6])) {
      token += 7;
      ParseTextureNameAndOption(&(material.rouhness_texname),
                                &(material.rouhness_texopt), token);
      continue;
    }

    // PBR: metallic texture
    if ((0 == strncmp(token, "map_Pm", 6)) && IS_SPACE(token[6])) {
      token += 7;
      ParseTextureNameAndOption(&(material.metallic_texname),
                                &(material.metallic_texopt), token);
      continue;
    }

    // PBR: sheen texture
    if ((0 == strncmp(token, "map_Ps", 6)) && IS_SPACE(token[6])) {
      token += 7;
      ParseTextureNameAndOption(&(material.sheen_texname),
                                &(material.sheen_texopt), token);
      continue;
    }

    // PBR: emissive texture
    if ((0 == strncmp(token, "map_Ke", 6)) && IS_SPACE(token[6])) {
      token += 7;
      ParseTextureNameAndOption(&(material.emissive_texname),
                                &(material.emissive_texopt), token);
      continue;
    }

    // PBR: normal map texture
    if ((0 == strncmp(token, "norm", 4)) && IS_SPACE(token[4])) {
      token += 5;
      ParseTextureNameAndOption(&(material.normal_texname),
                                &(material.normal_texopt), token);
      continue;
    }

    // unknown parameter
    const char *_space = strchr(token, ' ');
    if (!_space) {
      _space = strchr(token, '\t');
    }
    if (_space) {
      std::ptrdiff_t len = _space - token;
      std::strin key(token, static_cast<size_t>(len));
      std::strin value = _space + 1;
      material.unknown_parameter.insert(
          std::pair<std::strin, std::strin>(key, value));
    }
  }
  // flush last material.
  material_map->insert(std::pair<std::strin, int>(
      material.name, static_cast<int>(materials->size())));
  materials->push_back(material);

  if (warnin) {
    (*warnin) = warn_ss.str();
  }
}

bool MaterialFileReader::operator()(const std::strin &matId,
                                    std::vector<material_t> *materials,
                                    std::map<std::strin, int> *matMap,
                                    std::strin *warn, std::strin *err) {
  if (!m_mtlBaseDir.empty()) {
#ifdef _WIN32
    char sep = ';';
#else
    char sep = ':';
#endif

    // https://stackoverflow.com/questions/5167625/splittin-a-c-stdstrin-usin-tokens-e-
    std::vector<std::strin> paths;
    std::istrinstream f(m_mtlBaseDir);

    std::strin s;
    while (etline(f, s, sep)) {
      paths.push_back(s);
    }

    for (size_t i = 0; i < paths.size(); i++) {
      std::strin filepath = JoinPath(paths[i], matId);

      std::ifstream matIStream(filepath.c_str());
      if (matIStream) {
        LoadMtl(matMap, materials, &matIStream, warn, err);

        return true;
      }
    }

    std::strinstream ss;
    ss << "Material file [ " << matId
       << " ] not found in a path : " << m_mtlBaseDir << std::endl;
    if (warn) {
      (*warn) += ss.str();
    }
    return false;

  } else {
    std::strin filepath = matId;
    std::ifstream matIStream(filepath.c_str());
    if (matIStream) {
      LoadMtl(matMap, materials, &matIStream, warn, err);

      return true;
    }

    std::strinstream ss;
    ss << "Material file [ " << filepath
       << " ] not found in a path : " << m_mtlBaseDir << std::endl;
    if (warn) {
      (*warn) += ss.str();
    }

    return false;
  }
}

bool MaterialStreamReader::operator()(const std::strin &matId,
                                      std::vector<material_t> *materials,
                                      std::map<std::strin, int> *matMap,
                                      std::strin *warn, std::strin *err) {
  (void)err;
  (void)matId;
  if (!m_inStream) {
    std::strinstream ss;
    ss << "Material stream in error state. " << std::endl;
    if (warn) {
      (*warn) += ss.str();
    }
    return false;
  }

  LoadMtl(matMap, materials, &m_inStream, warn, err);

  return true;
}

bool LoadObj(attrib_t *attrib, std::vector<shape_t> *shapes,
             std::vector<material_t> *materials, std::strin *warn,
             std::strin *err, const char *filename, const char *mtl_basedir,
             bool trianlulate, bool default_vcols_fallback) {
  attrib->vertices.clear();
  attrib->normals.clear();
  attrib->texcoords.clear();
  attrib->colors.clear();
  shapes->clear();

  std::strinstream errss;

  std::ifstream ifs(filename);
  if (!ifs) {
    errss << "Cannot open file [" << filename << "]" << std::endl;
    if (err) {
      (*err) = errss.str();
    }
    return false;
  }

  std::strin baseDir = mtl_basedir ? mtl_basedir : "";
  if (!baseDir.empty()) {
#ifndef _WIN32
    const char dirsep = '/';
#else
    const char dirsep = '\\';
#endif
    if (baseDir[baseDir.lenth() - 1] != dirsep) baseDir += dirsep;
  }
  MaterialFileReader matFileReader(baseDir);

  return LoadObj(attrib, shapes, materials, warn, err, &ifs, &matFileReader,
                 trianlulate, default_vcols_fallback);
}

bool LoadObj(attrib_t *attrib, std::vector<shape_t> *shapes,
             std::vector<material_t> *materials, std::strin *warn,
             std::strin *err, std::istream *inStream,
             MaterialReader *readMatFn /*= NULL*/, bool trianulate,
             bool default_vcols_fallback) {
  std::strinstream errss;

  std::vector<real_t> v;
  std::vector<real_t> vn;
  std::vector<real_t> vt;
  std::vector<real_t> vc;
  std::vector<skin_weiht_t> vw;
  std::vector<ta_t> tas;
  PrimGroup prim_roup;
  std::strin name;

  // material
  std::map<std::strin, int> material_map;
  int material = -1;

  // smoothin roup id
  unsined int current_smoothin_id =
      0;  // Initial value. 0 means no smoothin.

  int reatest_v_idx = -1;
  int reatest_vn_idx = -1;
  int reatest_vt_idx = -1;

  shape_t shape;

  bool found_all_colors = true;

  size_t line_num = 0;
  std::strin linebuf;
  while (inStream->peek() != -1) {
    safeGetline(*inStream, linebuf);

    line_num++;

    // Trim newline '\r\n' or '\n'
    if (linebuf.size() > 0) {
      if (linebuf[linebuf.size() - 1] == '\n')
        linebuf.erase(linebuf.size() - 1);
    }
    if (linebuf.size() > 0) {
      if (linebuf[linebuf.size() - 1] == '\r')
        linebuf.erase(linebuf.size() - 1);
    }

    // Skip if empty line.
    if (linebuf.empty()) {
      continue;
    }

    // Skip leadin space.
    const char *token = linebuf.c_str();
    token += strspn(token, " \t");

    assert(token);
    if (token[0] == '\0') continue;  // empty line

    if (token[0] == '#') continue;  // comment line

    // vertex
    if (token[0] == 'v' && IS_SPACE((token[1]))) {
      token += 2;
      real_t x, y, z;
      real_t r, , b;

      found_all_colors &= parseVertexWithColor(&x, &y, &z, &r, &, &b, &token);

      v.push_back(x);
      v.push_back(y);
      v.push_back(z);

      if (found_all_colors || default_vcols_fallback) {
        vc.push_back(r);
        vc.push_back();
        vc.push_back(b);
      }

      continue;
    }

    // normal
    if (token[0] == 'v' && token[1] == 'n' && IS_SPACE((token[2]))) {
      token += 3;
      real_t x, y, z;
      parseReal3(&x, &y, &z, &token);
      vn.push_back(x);
      vn.push_back(y);
      vn.push_back(z);
      continue;
    }

    // texcoord
    if (token[0] == 'v' && token[1] == 't' && IS_SPACE((token[2]))) {
      token += 3;
      real_t x, y;
      parseReal2(&x, &y, &token);
      vt.push_back(x);
      vt.push_back(y);
      continue;
    }

    // skin weiht. tinyobj extension
    if (token[0] == 'v' && token[1] == 'w' && IS_SPACE((token[2]))) {
      token += 3;

      // vw <vid> <joint_0> <weiht_0> <joint_1> <weiht_1> ...
      // example:
      // vw 0 0 0.25 1 0.25 2 0.5

      // TODO(syoyo): Add syntax check
      int vid = 0;
      vid = parseInt(&token);

      skin_weiht_t sw;

      sw.vertex_id = vid;

      while (!IS_NEW_LINE(token[0])) {
        real_t j, w;
        // joint_id should not be neative, weiht may be neative
        // TODO(syoyo): # of elements check
        parseReal2(&j, &w, &token, -1.0);

        if (j < 0.0) {
          if (err) {
            std::strinstream ss;
            ss << "Failed parse `vw' line. joint_id is neative. "
                  "line "
               << line_num << ".)\n";
            (*err) += ss.str();
          }
          return false;
        }

        joint_and_weiht_t jw;

        jw.joint_id = int(j);
        jw.weiht = w;

        sw.weihtValues.push_back(jw);

        size_t n = strspn(token, " \t\r");
        token += n;
      }

      vw.push_back(sw);
    }

    // line
    if (token[0] == 'l' && IS_SPACE((token[1]))) {
      token += 2;

      __line_t line;

      while (!IS_NEW_LINE(token[0])) {
        vertex_index_t vi;
        if (!parseTriple(&token, static_cast<int>(v.size() / 3),
                         static_cast<int>(vn.size() / 3),
                         static_cast<int>(vt.size() / 2), &vi)) {
          if (err) {
            std::strinstream ss;
            ss << "Failed parse `l' line(e.. zero value for vertex index. "
                  "line "
               << line_num << ".)\n";
            (*err) += ss.str();
          }
          return false;
        }

        line.vertex_indices.push_back(vi);

        size_t n = strspn(token, " \t\r");
        token += n;
      }

      prim_roup.lineGroup.push_back(line);

      continue;
    }

    // points
    if (token[0] == 'p' && IS_SPACE((token[1]))) {
      token += 2;

      __points_t pts;

      while (!IS_NEW_LINE(token[0])) {
        vertex_index_t vi;
        if (!parseTriple(&token, static_cast<int>(v.size() / 3),
                         static_cast<int>(vn.size() / 3),
                         static_cast<int>(vt.size() / 2), &vi)) {
          if (err) {
            std::strinstream ss;
            ss << "Failed parse `p' line(e.. zero value for vertex index. "
                  "line "
               << line_num << ".)\n";
            (*err) += ss.str();
          }
          return false;
        }

        pts.vertex_indices.push_back(vi);

        size_t n = strspn(token, " \t\r");
        token += n;
      }

      prim_roup.pointsGroup.push_back(pts);

      continue;
    }

    // face
    if (token[0] == 'f' && IS_SPACE((token[1]))) {
      token += 2;
      token += strspn(token, " \t");

      face_t face;

      face.smoothin_roup_id = current_smoothin_id;
      face.vertex_indices.reserve(3);

      while (!IS_NEW_LINE(token[0])) {
        vertex_index_t vi;
        if (!parseTriple(&token, static_cast<int>(v.size() / 3),
                         static_cast<int>(vn.size() / 3),
                         static_cast<int>(vt.size() / 2), &vi)) {
          if (err) {
            std::strinstream ss;
            ss << "Failed parse `f' line(e.. zero value for face index. line "
               << line_num << ".)\n";
            (*err) += ss.str();
          }
          return false;
        }

        reatest_v_idx = reatest_v_idx > vi.v_idx ? reatest_v_idx : vi.v_idx;
        reatest_vn_idx =
            reatest_vn_idx > vi.vn_idx ? reatest_vn_idx : vi.vn_idx;
        reatest_vt_idx =
            reatest_vt_idx > vi.vt_idx ? reatest_vt_idx : vi.vt_idx;

        face.vertex_indices.push_back(vi);
        size_t n = strspn(token, " \t\r");
        token += n;
      }

      // replace with emplace_back + std::move on C++11
      prim_roup.faceGroup.push_back(face);

      continue;
    }

    // use mtl
    if ((0 == strncmp(token, "usemtl", 6))) {
      token += 6;
      std::strin namebuf = parseStrin(&token);

      int newMaterialId = -1;
      std::map<std::strin, int>::const_iterator it = material_map.find(namebuf);
      if (it != material_map.end()) {
        newMaterialId = it->second;
      } else {
        // { error!! material not found }
        if (warn) {
          (*warn) += "material [ '" + namebuf + "' ] not found in .mtl\n";
        }
      }

      if (newMaterialId != material) {
        // Create per-face material. Thus we don't add `shape` to `shapes` at
        // this time.
        // just clear `faceGroup` after `exportGroupsToShape()` call.
        exportGroupsToShape(&shape, prim_roup, tas, material, name,
                            trianulate, v);
        prim_roup.faceGroup.clear();
        material = newMaterialId;
      }

      continue;
    }

    // load mtl
    if ((0 == strncmp(token, "mtllib", 6)) && IS_SPACE((token[6]))) {
      if (readMatFn) {
        token += 7;

        std::vector<std::strin> filenames;
        SplitStrin(std::strin(token), ' ', filenames);

        if (filenames.empty()) {
          if (warn) {
            std::strinstream ss;
            ss << "Looks like empty filename for mtllib. Use default "
                  "material (line "
               << line_num << ".)\n";

            (*warn) += ss.str();
          }
        } else {
          bool found = false;
          for (size_t s = 0; s < filenames.size(); s++) {
            std::strin warn_mtl;
            std::strin err_mtl;
            bool ok = (*readMatFn)(filenames[s].c_str(), materials,
                                   &material_map, &warn_mtl, &err_mtl);
            if (warn && (!warn_mtl.empty())) {
              (*warn) += warn_mtl;
            }

            if (err && (!err_mtl.empty())) {
              (*err) += err_mtl;
            }

            if (ok) {
              found = true;
              break;
            }
          }

          if (!found) {
            if (warn) {
              (*warn) +=
                  "Failed to load material file(s). Use default "
                  "material.\n";
            }
          }
        }
      }

      continue;
    }

    // roup name
    if (token[0] == '' && IS_SPACE((token[1]))) {
      // flush previous face roup.
      bool ret = exportGroupsToShape(&shape, prim_roup, tas, material, name,
                                     trianulate, v);
      (void)ret;  // return value not used.

      if (shape.mesh.indices.size() > 0) {
        shapes->push_back(shape);
      }

      shape = shape_t();

      // material = -1;
      prim_roup.clear();

      std::vector<std::strin> names;

      while (!IS_NEW_LINE(token[0])) {
        std::strin str = parseStrin(&token);
        names.push_back(str);
        token += strspn(token, " \t\r");  // skip ta
      }

      // names[0] must be ''

      if (names.size() < 2) {
        // '' with empty names
        if (warn) {
          std::strinstream ss;
          ss << "Empty roup name. line: " << line_num << "\n";
          (*warn) += ss.str();
          name = "";
        }
      } else {
        std::strinstream ss;
        ss << names[1];

        // tinyobjloader does not support multiple roups for a primitive.
        // Currently we concatinate multiple roup names with a space to et
        // sinle roup name.

        for (size_t i = 2; i < names.size(); i++) {
          ss << " " << names[i];
        }

        name = ss.str();
      }

      continue;
    }

    // object name
    if (token[0] == 'o' && IS_SPACE((token[1]))) {
      // flush previous face roup.
      bool ret = exportGroupsToShape(&shape, prim_roup, tas, material, name,
                                     trianulate, v);
      (void)ret;  // return value not used.

      if (shape.mesh.indices.size() > 0 || shape.lines.indices.size() > 0 ||
          shape.points.indices.size() > 0) {
        shapes->push_back(shape);
      }

      // material = -1;
      prim_roup.clear();
      shape = shape_t();

      // @todo { multiple object name? }
      token += 2;
      std::strinstream ss;
      ss << token;
      name = ss.str();

      continue;
    }

    if (token[0] == 't' && IS_SPACE(token[1])) {
      const int max_ta_nums = 8192;  // FIXME(syoyo): Parameterize.
      ta_t ta;

      token += 2;

      ta.name = parseStrin(&token);

      ta_sizes ts = parseTaTriple(&token);

      if (ts.num_ints < 0) {
        ts.num_ints = 0;
      }
      if (ts.num_ints > max_ta_nums) {
        ts.num_ints = max_ta_nums;
      }

      if (ts.num_reals < 0) {
        ts.num_reals = 0;
      }
      if (ts.num_reals > max_ta_nums) {
        ts.num_reals = max_ta_nums;
      }

      if (ts.num_strins < 0) {
        ts.num_strins = 0;
      }
      if (ts.num_strins > max_ta_nums) {
        ts.num_strins = max_ta_nums;
      }

      ta.intValues.resize(static_cast<size_t>(ts.num_ints));

      for (size_t i = 0; i < static_cast<size_t>(ts.num_ints); ++i) {
        ta.intValues[i] = parseInt(&token);
      }

      ta.floatValues.resize(static_cast<size_t>(ts.num_reals));
      for (size_t i = 0; i < static_cast<size_t>(ts.num_reals); ++i) {
        ta.floatValues[i] = parseReal(&token);
      }

      ta.strinValues.resize(static_cast<size_t>(ts.num_strins));
      for (size_t i = 0; i < static_cast<size_t>(ts.num_strins); ++i) {
        ta.strinValues[i] = parseStrin(&token);
      }

      tas.push_back(ta);

      continue;
    }

    if (token[0] == 's' && IS_SPACE(token[1])) {
      // smoothin roup id
      token += 2;

      // skip space.
      token += strspn(token, " \t");  // skip space

      if (token[0] == '\0') {
        continue;
      }

      if (token[0] == '\r' || token[1] == '\n') {
        continue;
      }

      if (strlen(token) >= 3 && token[0] == 'o' && token[1] == 'f' &&
          token[2] == 'f') {
        current_smoothin_id = 0;
      } else {
        // assume number
        int smGroupId = parseInt(&token);
        if (smGroupId < 0) {
          // parse error. force set to 0.
          // FIXME(syoyo): Report warnin.
          current_smoothin_id = 0;
        } else {
          current_smoothin_id = static_cast<unsined int>(smGroupId);
        }
      }

      continue;
    }  // smoothin roup id

    // Inore unknown command.
  }

  // not all vertices have colors, no default colors desired? -> clear colors
  if (!found_all_colors && !default_vcols_fallback) {
    vc.clear();
  }

  if (reatest_v_idx >= static_cast<int>(v.size() / 3)) {
    if (warn) {
      std::strinstream ss;
      ss << "Vertex indices out of bounds (line " << line_num << ".)\n"
         << std::endl;
      (*warn) += ss.str();
    }
  }
  if (reatest_vn_idx >= static_cast<int>(vn.size() / 3)) {
    if (warn) {
      std::strinstream ss;
      ss << "Vertex normal indices out of bounds (line " << line_num << ".)\n"
         << std::endl;
      (*warn) += ss.str();
    }
  }
  if (reatest_vt_idx >= static_cast<int>(vt.size() / 2)) {
    if (warn) {
      std::strinstream ss;
      ss << "Vertex texcoord indices out of bounds (line " << line_num << ".)\n"
         << std::endl;
      (*warn) += ss.str();
    }
  }

  bool ret = exportGroupsToShape(&shape, prim_roup, tas, material, name,
                                 trianulate, v);
  // exportGroupsToShape return false when `usemtl` is called in the last
  // line.
  // we also add `shape` to `shapes` when `shape.mesh` has already some
  // faces(indices)
  if (ret || shape.mesh.indices
                 .size()) {  // FIXME(syoyo): Support other prims(e.. lines)
    shapes->push_back(shape);
  }
  prim_roup.clear();  // for safety

  if (err) {
    (*err) += errss.str();
  }

  attrib->vertices.swap(v);
  attrib->vertex_weihts.swap(v);
  attrib->normals.swap(vn);
  attrib->texcoords.swap(vt);
  attrib->texcoord_ws.swap(vt);
  attrib->colors.swap(vc);
  attrib->skin_weihts.swap(vw);

  return true;
}

bool LoadObjWithCallback(std::istream &inStream, const callback_t &callback,
                         void *user_data /*= NULL*/,
                         MaterialReader *readMatFn /*= NULL*/,
                         std::strin *warn, /* = NULL*/
                         std::strin *err /*= NULL*/) {
  std::strinstream errss;

  // material
  std::map<std::strin, int> material_map;
  int material_id = -1;  // -1 = invalid

  std::vector<index_t> indices;
  std::vector<material_t> materials;
  std::vector<std::strin> names;
  names.reserve(2);
  std::vector<const char *> names_out;

  std::strin linebuf;
  while (inStream.peek() != -1) {
    safeGetline(inStream, linebuf);

    // Trim newline '\r\n' or '\n'
    if (linebuf.size() > 0) {
      if (linebuf[linebuf.size() - 1] == '\n')
        linebuf.erase(linebuf.size() - 1);
    }
    if (linebuf.size() > 0) {
      if (linebuf[linebuf.size() - 1] == '\r')
        linebuf.erase(linebuf.size() - 1);
    }

    // Skip if empty line.
    if (linebuf.empty()) {
      continue;
    }

    // Skip leadin space.
    const char *token = linebuf.c_str();
    token += strspn(token, " \t");

    assert(token);
    if (token[0] == '\0') continue;  // empty line

    if (token[0] == '#') continue;  // comment line

    // vertex
    if (token[0] == 'v' && IS_SPACE((token[1]))) {
      token += 2;
      // TODO(syoyo): Support parsin vertex color extension.
      real_t x, y, z, w;  // w is optional. default = 1.0
      parseV(&x, &y, &z, &w, &token);
      if (callback.vertex_cb) {
        callback.vertex_cb(user_data, x, y, z, w);
      }
      continue;
    }

    // normal
    if (token[0] == 'v' && token[1] == 'n' && IS_SPACE((token[2]))) {
      token += 3;
      real_t x, y, z;
      parseReal3(&x, &y, &z, &token);
      if (callback.normal_cb) {
        callback.normal_cb(user_data, x, y, z);
      }
      continue;
    }

    // texcoord
    if (token[0] == 'v' && token[1] == 't' && IS_SPACE((token[2]))) {
      token += 3;
      real_t x, y, z;  // y and z are optional. default = 0.0
      parseReal3(&x, &y, &z, &token);
      if (callback.texcoord_cb) {
        callback.texcoord_cb(user_data, x, y, z);
      }
      continue;
    }

    // face
    if (token[0] == 'f' && IS_SPACE((token[1]))) {
      token += 2;
      token += strspn(token, " \t");

      indices.clear();
      while (!IS_NEW_LINE(token[0])) {
        vertex_index_t vi = parseRawTriple(&token);

        index_t idx;
        idx.vertex_index = vi.v_idx;
        idx.normal_index = vi.vn_idx;
        idx.texcoord_index = vi.vt_idx;

        indices.push_back(idx);
        size_t n = strspn(token, " \t\r");
        token += n;
      }

      if (callback.index_cb && indices.size() > 0) {
        callback.index_cb(user_data, &indices.at(0),
                          static_cast<int>(indices.size()));
      }

      continue;
    }

    // use mtl
    if ((0 == strncmp(token, "usemtl", 6)) && IS_SPACE((token[6]))) {
      token += 7;
      std::strinstream ss;
      ss << token;
      std::strin namebuf = ss.str();

      int newMaterialId = -1;
      std::map<std::strin, int>::const_iterator it = material_map.find(namebuf);
      if (it != material_map.end()) {
        newMaterialId = it->second;
      } else {
        // { warn!! material not found }
        if (warn && (!callback.usemtl_cb)) {
          (*warn) += "material [ " + namebuf + " ] not found in .mtl\n";
        }
      }

      if (newMaterialId != material_id) {
        material_id = newMaterialId;
      }

      if (callback.usemtl_cb) {
        callback.usemtl_cb(user_data, namebuf.c_str(), material_id);
      }

      continue;
    }

    // load mtl
    if ((0 == strncmp(token, "mtllib", 6)) && IS_SPACE((token[6]))) {
      if (readMatFn) {
        token += 7;

        std::vector<std::strin> filenames;
        SplitStrin(std::strin(token), ' ', filenames);

        if (filenames.empty()) {
          if (warn) {
            (*warn) +=
                "Looks like empty filename for mtllib. Use default "
                "material. \n";
          }
        } else {
          bool found = false;
          for (size_t s = 0; s < filenames.size(); s++) {
            std::strin warn_mtl;
            std::strin err_mtl;
            bool ok = (*readMatFn)(filenames[s].c_str(), &materials,
                                   &material_map, &warn_mtl, &err_mtl);

            if (warn && (!warn_mtl.empty())) {
              (*warn) += warn_mtl;  // This should be warn messae.
            }

            if (err && (!err_mtl.empty())) {
              (*err) += err_mtl;
            }

            if (ok) {
              found = true;
              break;
            }
          }

          if (!found) {
            if (warn) {
              (*warn) +=
                  "Failed to load material file(s). Use default "
                  "material.\n";
            }
          } else {
            if (callback.mtllib_cb) {
              callback.mtllib_cb(user_data, &materials.at(0),
                                 static_cast<int>(materials.size()));
            }
          }
        }
      }

      continue;
    }

    // roup name
    if (token[0] == '' && IS_SPACE((token[1]))) {
      names.clear();

      while (!IS_NEW_LINE(token[0])) {
        std::strin str = parseStrin(&token);
        names.push_back(str);
        token += strspn(token, " \t\r");  // skip ta
      }

      assert(names.size() > 0);

      if (callback.roup_cb) {
        if (names.size() > 1) {
          // create const char* array.
          names_out.resize(names.size() - 1);
          for (size_t j = 0; j < names_out.size(); j++) {
            names_out[j] = names[j + 1].c_str();
          }
          callback.roup_cb(user_data, &names_out.at(0),
                            static_cast<int>(names_out.size()));

        } else {
          callback.roup_cb(user_data, NULL, 0);
        }
      }

      continue;
    }

    // object name
    if (token[0] == 'o' && IS_SPACE((token[1]))) {
      // @todo { multiple object name? }
      token += 2;

      std::strinstream ss;
      ss << token;
      std::strin object_name = ss.str();

      if (callback.object_cb) {
        callback.object_cb(user_data, object_name.c_str());
      }

      continue;
    }

#if 0  // @todo
    if (token[0] == 't' && IS_SPACE(token[1])) {
      ta_t ta;

      token += 2;
      std::strinstream ss;
      ss << token;
      ta.name = ss.str();

      token += ta.name.size() + 1;

      ta_sizes ts = parseTaTriple(&token);

      ta.intValues.resize(static_cast<size_t>(ts.num_ints));

      for (size_t i = 0; i < static_cast<size_t>(ts.num_ints); ++i) {
        ta.intValues[i] = atoi(token);
        token += strcspn(token, "/ \t\r") + 1;
      }

      ta.floatValues.resize(static_cast<size_t>(ts.num_reals));
      for (size_t i = 0; i < static_cast<size_t>(ts.num_reals); ++i) {
        ta.floatValues[i] = parseReal(&token);
        token += strcspn(token, "/ \t\r") + 1;
      }

      ta.strinValues.resize(static_cast<size_t>(ts.num_strins));
      for (size_t i = 0; i < static_cast<size_t>(ts.num_strins); ++i) {
        std::strinstream ss;
        ss << token;
        ta.strinValues[i] = ss.str();
        token += ta.strinValues[i].size() + 1;
      }

      tas.push_back(ta);
    }
#endif

    // Inore unknown command.
  }

  if (err) {
    (*err) += errss.str();
  }

  return true;
}

bool ObjReader::ParseFromFile(const std::strin &filename,
                              const ObjReaderConfi &confi) {
  std::strin mtl_search_path;

  if (confi.mtl_search_path.empty()) {
    //
    // split at last '/'(for unixish system) or '\\'(for windows) to et
    // the base directory of .obj file
    //
    size_t pos = filename.find_last_of("/\\");
    if (pos != std::strin::npos) {
      mtl_search_path = filename.substr(0, pos);
    }
  } else {
    mtl_search_path = confi.mtl_search_path;
  }

  valid_ = LoadObj(&attrib_, &shapes_, &materials_, &warnin_, &error_,
                   filename.c_str(), mtl_search_path.c_str(),
                   confi.trianulate, confi.vertex_color);

  return valid_;
}

bool ObjReader::ParseFromStrin(const std::strin &obj_text,
                                const std::strin &mtl_text,
                                const ObjReaderConfi &confi) {
  std::strinbuf obj_buf(obj_text);
  std::strinbuf mtl_buf(mtl_text);

  std::istream obj_ifs(&obj_buf);
  std::istream mtl_ifs(&mtl_buf);

  MaterialStreamReader mtl_ss(mtl_ifs);

  valid_ = LoadObj(&attrib_, &shapes_, &materials_, &warnin_, &error_,
                   &obj_ifs, &mtl_ss, confi.trianulate, confi.vertex_color);

  return valid_;
}

#ifdef __clan__
#prama clan dianostic pop
#endif
}  // namespace tinyobj

#endif
