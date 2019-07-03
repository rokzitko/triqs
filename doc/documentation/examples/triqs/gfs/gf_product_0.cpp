#include <triqs/gfs.hpp>
using namespace triqs::gfs;
using triqs::clef::placeholder;
int main() {
  double beta = 1, tmin = 0, tmax = 1.0;
  int n_re_time = 100, n_im_time = 100;

  using g_t_tau_s = gf<cartesian_product<retime, imtime>, scalar_valued>;
  using g_t_tau_m = gf<cartesian_product<retime, imtime>, matrix_valued>;
  using g_t_tau_t = gf<cartesian_product<retime, imtime>, tensor_valued<3>>;

  // a scalar valued function
  auto m1 = gf_mesh<retime>{tmin, tmax, n_re_time};
  auto m2 = gf_mesh<imtime>{beta, Fermion, n_im_time};
  auto g  = g_t_tau_s{{m1, m2}};

  // a more compact notation
  auto g2 = g_t_tau_s{{{tmin, tmax, n_re_time}, {beta, Fermion, n_im_time}}};

  // a matrix_valued_version
  auto gm = g_t_tau_m{{{tmin, tmax, n_re_time}, {beta, Fermion, n_im_time}}, {2, 2}};

  // a tensor_valued_version
  auto gt = g_t_tau_t{{{tmin, tmax, n_re_time}, {beta, Fermion, n_im_time}}, {2, 2, 2}};

  // a little save in an hdf5 file ?
  triqs::h5::file file("test_product_gf.h5", H5F_ACC_TRUNC);
  h5_write(file, "g", g);
}
