#include "hermes2d.h"

namespace WeakFormsH1 
{
  template<typename Scalar>
  DefaultMatrixFormVol<Scalar>::DefaultMatrixFormVol<Scalar>
    (int i, int j, std::string area, Scalar const_coeff, 
    DefaultFunction<Scalar>* f_coeff, SymFlag sym, 
    GeomType gt)
    : MatrixFormVol<Scalar>(i, j, area, sym), const_coeff(const_coeff), function_coeff(f_coeff), gt(gt)
  {
    // If f_coeff is HERMES_DEFAULT_FUNCTION, initialize it to be constant 1.0.
    if (f_coeff == HERMES_DEFAULT_FUNCTION) this->function_coeff = new DefaultFunction<Scalar>(1.0);
  }

  template<typename Scalar>
  DefaultMatrixFormVol<Scalar>::DefaultMatrixFormVol<Scalar>
    (int i, int j, Hermes::vector<std::string> areas,Scalar const_coeff, 
    DefaultFunction<Scalar>* f_coeff, SymFlag sym, GeomType gt)
    : MatrixFormVol<Scalar>(i, j, areas, sym), const_coeff(const_coeff), function_coeff(f_coeff), gt(gt)
  {
    // If f_coeff is HERMES_DEFAULT_FUNCTION, initialize it to be constant 1.0.
    if (f_coeff == HERMES_DEFAULT_FUNCTION) this->function_coeff = new DefaultFunction<Scalar>(1.0);
  }

  template<typename Scalar>
  DefaultMatrixFormVol<Scalar>::~DefaultMatrixFormVol<Scalar>() 
  {
    if (function_coeff != HERMES_DEFAULT_FUNCTION) delete function_coeff;
  };

  template<typename Scalar>
  Scalar DefaultMatrixFormVol<Scalar>::value(int n, double *wt, Func<Scalar> *u_ext[], Func<double> *u, Func<double> *v,
    Geom<double> *e, ExtData<Scalar> *ext) const 
  {
    Scalar result = 0;
    if (gt == HERMES_PLANAR) 
    {
      for (int i = 0; i < n; i++) 
      {
        result += wt[i] * function_coeff->value(e->x[i], e->y[i]) * u->val[i] * v->val[i];
      }
    }
    else 
    {
      if (gt == HERMES_AXISYM_X) 
      {
        for (int i = 0; i < n; i++) 
        {
          result += wt[i] * e->y[i] * function_coeff->value(e->x[i], e->y[i]) * u->val[i] * v->val[i];
        }
      }
      else 
      {
        for (int i = 0; i < n; i++) 
        {
          result += wt[i] * e->x[i] * function_coeff->value(e->x[i], e->y[i]) * u->val[i] * v->val[i];
        }
      }
    }

    return const_coeff * result;
  }

  template<typename Scalar>
  Ord DefaultMatrixFormVol<Scalar>::ord(int n, double *wt, Func<Ord> *u_ext[], Func<Ord> *u,
    Func<Ord> *v, Geom<Ord> *e, ExtData<Ord> *ext) const 
  {
    Ord result = 0;
    if (gt == HERMES_PLANAR) 
    {
      for (int i = 0; i < n; i++) 
      {
        result += wt[i] * function_coeff->ord(e->x[i], e->y[i]) * u->val[i] * v->val[i];
      }
    }
    else 
    {
      if (gt == HERMES_AXISYM_X) 
      {
        for (int i = 0; i < n; i++) 
        {
          result += wt[i] * e->y[i] * function_coeff->ord(e->x[i], e->y[i]) * u->val[i] * v->val[i];
        }
      }
      else 
      {
        for (int i = 0; i < n; i++) 
        {
          result += wt[i] * e->x[i] * function_coeff->ord(e->x[i], e->y[i]) * u->val[i] * v->val[i];
        }
      }
    }

    return result;
  }

  template<typename Scalar>
  MatrixFormVol<Scalar>* DefaultMatrixFormVol<Scalar>::clone() 
  {
    return new DefaultMatrixFormVol<Scalar>(*this);
  }


  template<typename Scalar>
  DefaultJacobianDiffusion<Scalar>::DefaultJacobianDiffusion(int i, int j, std::string area, Scalar const_coeff,
    CubicSpline* c_spline,
    SymFlag sym, GeomType gt)
    : MatrixFormVol<Scalar>(i, j, area, sym), 
    idx_j(j), const_coeff(const_coeff), spline_coeff(c_spline), gt(gt)
  {
    // If spline is HERMES_DEFAULT_SPLINE, initialize it to be constant 1.0.
    if (c_spline == HERMES_DEFAULT_SPLINE) this->spline_coeff = new CubicSpline(1.0);
  };

  template<typename Scalar>
  DefaultJacobianDiffusion<Scalar>::DefaultJacobianDiffusion(int i, int j, Hermes::vector<std::string> areas, 
    Scalar const_coeff, CubicSpline* c_spline,
    SymFlag sym, GeomType gt)
    : MatrixFormVol<Scalar>(i, j, areas, sym),
    idx_j(j), const_coeff(const_coeff), spline_coeff(c_spline), gt(gt)
  {
    // If spline is HERMES_DEFAULT_SPLINE, initialize it to be constant 1.0.
    if (c_spline == HERMES_DEFAULT_SPLINE) this->spline_coeff = new CubicSpline(1.0);
  }

  template<typename Scalar>
  DefaultJacobianDiffusion<Scalar>::~DefaultJacobianDiffusion() 
  {
    if (spline_coeff != HERMES_DEFAULT_SPLINE) delete spline_coeff;
  };

  template<typename Scalar>
  Scalar DefaultJacobianDiffusion<Scalar>::value(int n, double *wt, Func<Scalar> *u_ext[], Func<double> *u,
    Func<double> *v, Geom<double> *e, ExtData<Scalar> *ext) const
  {
    Scalar result = 0;
    if (gt == HERMES_PLANAR) 
    {
      for (int i = 0; i < n; i++) 
      {
        result += wt[i] * (const_coeff*spline_coeff->get_derivative(u_ext[idx_j]->val[i]) * u->val[i] *
          (u_ext[idx_j]->dx[i] * v->dx[i] + u_ext[idx_j]->dy[i] * v->dy[i])
          + const_coeff*spline_coeff->get_value(u_ext[idx_j]->val[i])
          * (u->dx[i] * v->dx[i] + u->dy[i] * v->dy[i]));
      }
    }
    else 
    {
      if (gt == HERMES_AXISYM_X) 
      {
        for (int i = 0; i < n; i++) 
        {
          result += wt[i] * e->y[i] * (const_coeff*spline_coeff->get_derivative(u_ext[idx_j]->val[i]) * u->val[i] *
            (u_ext[idx_j]->dx[i] * v->dx[i] + u_ext[idx_j]->dy[i] * v->dy[i])
            + const_coeff*spline_coeff->get_value(u_ext[idx_j]->val[i])
            * (u->dx[i] * v->dx[i] + u->dy[i] * v->dy[i]));
        }
      }
      else 
      {
        for (int i = 0; i < n; i++) 
        {
          result += wt[i] * e->x[i] * (const_coeff*spline_coeff->get_derivative(u_ext[idx_j]->val[i]) * u->val[i] *
            (u_ext[idx_j]->dx[i] * v->dx[i] + u_ext[idx_j]->dy[i] * v->dy[i])
            + const_coeff*spline_coeff->get_value(u_ext[idx_j]->val[i])
            * (u->dx[i] * v->dx[i] + u->dy[i] * v->dy[i]));
        }
      }
    }

    return result;
  }

  template<typename Scalar>
  Ord DefaultJacobianDiffusion<Scalar>::ord(int n, double *wt, Func<Ord> *u_ext[], Func<Ord> *u, Func<Ord> *v,
    Geom<Ord> *e, ExtData<Ord> *ext) const
  {
    Ord result = 0;
    if (gt == HERMES_PLANAR) 
    {
      for (int i = 0; i < n; i++) 
      {
        result += wt[i] * (const_coeff*spline_coeff->get_derivative(u_ext[idx_j]->val[i]) * u->val[i] *
          (u_ext[idx_j]->dx[i] * v->dx[i] + u_ext[idx_j]->dy[i] * v->dy[i])
          + const_coeff*spline_coeff->get_value(u_ext[idx_j]->val[i])
          * (u->dx[i] * v->dx[i] + u->dy[i] * v->dy[i]));
      }
    }
    else 
    {
      if (gt == HERMES_AXISYM_X) 
      {
        for (int i = 0; i < n; i++) 
        {
          result += wt[i] * e->y[i] * (const_coeff*spline_coeff->get_derivative(u_ext[idx_j]->val[i]) * u->val[i] *
            (u_ext[idx_j]->dx[i] * v->dx[i] + u_ext[idx_j]->dy[i] * v->dy[i])
            + const_coeff*spline_coeff->get_value(u_ext[idx_j]->val[i])
            * (u->dx[i] * v->dx[i] + u->dy[i] * v->dy[i]));
        }
      }
      else 
      {
        for (int i = 0; i < n; i++) 
        {
          result += wt[i] * e->x[i] * (const_coeff*spline_coeff->get_derivative(u_ext[idx_j]->val[i]) * u->val[i] *
            (u_ext[idx_j]->dx[i] * v->dx[i] + u_ext[idx_j]->dy[i] * v->dy[i])
            + const_coeff*spline_coeff->get_value(u_ext[idx_j]->val[i])
            * (u->dx[i] * v->dx[i] + u->dy[i] * v->dy[i]));
        }
      }
    }

    return result;
  }

  template<typename Scalar>
  MatrixFormVol<Scalar>* DefaultJacobianDiffusion<Scalar>::clone() 
  {
    return new DefaultJacobianDiffusion<Scalar>(*this);
  }


  template<typename Scalar>
  DefaultJacobianAdvection<Scalar>::DefaultJacobianAdvection(int i, int j, std::string area, 
    Scalar const_coeff1, Scalar const_coeff2,
    CubicSpline* c_spline1,
    CubicSpline* c_spline2, GeomType gt)
    : MatrixFormVol<Scalar>(i, j, area, HERMES_NONSYM),
    idx_j(j), const_coeff1(const_coeff1), const_coeff2(const_coeff2), 
    spline_coeff1(c_spline1), spline_coeff2(c_spline2), gt(gt)
  {
    if (gt != HERMES_PLANAR) error("Axisymmetric advection forms not implemented yet.");

    // If spline1 == HERMES_DEFAULT_SPLINE or spline2 == HERMES_DEFAULT_SPLINE, initialize it to be constant 1.0.
    if (c_spline1 == HERMES_DEFAULT_SPLINE) this->spline_coeff1 = new CubicSpline(1.0);
    if (c_spline2 == HERMES_DEFAULT_SPLINE) this->spline_coeff2 = new CubicSpline(1.0);
  }

  template<typename Scalar>
  DefaultJacobianAdvection<Scalar>::DefaultJacobianAdvection(int i, int j, Hermes::vector<std::string> areas, 
    Scalar const_coeff1, Scalar const_coeff2,
    CubicSpline* c_spline1,
    CubicSpline* c_spline2,
    GeomType gt)
    : MatrixFormVol<Scalar>(i, j, areas, HERMES_NONSYM),
    idx_j(j), const_coeff1(const_coeff1), const_coeff2(const_coeff2), 
    spline_coeff1(spline_coeff1), spline_coeff2(spline_coeff2), gt(gt)
  {
    if (gt != HERMES_PLANAR) error("Axisymmetric advection forms not implemented yet.");

    // If spline1 == HERMES_DEFAULT_SPLINE or spline2 == HERMES_DEFAULT_SPLINE, initialize it to be constant 1.0.
    if (c_spline1 == HERMES_DEFAULT_SPLINE) this->spline_coeff1 = new CubicSpline(1.0);
    if (c_spline2 == HERMES_DEFAULT_SPLINE) this->spline_coeff2 = new CubicSpline(1.0);
  }

  template<typename Scalar>
  DefaultJacobianAdvection<Scalar>::~DefaultJacobianAdvection() 
  {
    if (spline_coeff1 != HERMES_DEFAULT_SPLINE) delete spline_coeff1;
    if (spline_coeff2 != HERMES_DEFAULT_SPLINE) delete spline_coeff2;
  };

  template<typename Scalar>
  Scalar DefaultJacobianAdvection<Scalar>::value(int n, double *wt, Func<Scalar> *u_ext[], Func<double> *u,
    Func<double> *v, Geom<double> *e, ExtData<Scalar> *ext) const 
  {
    Scalar result = 0;
    for (int i = 0; i < n; i++) 
    {
      result += wt[i] * (  const_coeff1*spline_coeff1->get_derivative(u_ext[idx_j]->val[i]) * u->val[i] * u_ext[idx_j]->dx[i] * v->val[i]
      + const_coeff1*spline_coeff1->get_value(u_ext[idx_j]->val[i]) * u->dx[i] * v->val[i]
      + const_coeff2*spline_coeff2->get_derivative(u_ext[idx_j]->val[i]) * u->val[i] * u_ext[idx_j]->dy[i] * v->val[i]
      + const_coeff2*spline_coeff2->get_value(u_ext[idx_j]->val[i]) * u->dy[i] * v->val[i]);
    }
    return result;
  }

  template<typename Scalar>
  Ord DefaultJacobianAdvection<Scalar>::ord(int n, double *wt, Func<Ord> *u_ext[], Func<Ord> *u, Func<Ord> *v,
    Geom<Ord> *e, ExtData<Ord> *ext) const 
  {
    Ord result = 0;
    for (int i = 0; i < n; i++) 
    {
      result += wt[i] * (  const_coeff1*spline_coeff1->get_derivative(u_ext[idx_j]->val[i]) * u->val[i] * u_ext[idx_j]->dx[i] * v->val[i]
      + const_coeff1*spline_coeff1->get_value(u_ext[idx_j]->val[i]) * u->dx[i] * v->val[i]
      + const_coeff2*spline_coeff2->get_derivative(u_ext[idx_j]->val[i]) * u->val[i] * u_ext[idx_j]->dy[i] * v->val[i]
      + const_coeff2*spline_coeff2->get_value(u_ext[idx_j]->val[i]) * u->dy[i] * v->val[i]);
    }
    return result;
  }

  template<typename Scalar>
  MatrixFormVol<Scalar>* DefaultJacobianAdvection<Scalar>::clone() 
  {
    return new DefaultJacobianAdvection<Scalar>(*this);
  }


  template<typename Scalar>
  DefaultVectorFormVol<Scalar>::DefaultVectorFormVol<Scalar>(int i, std::string area, Scalar const_coeff,
    DefaultFunction<Scalar>* f_coeff,
    GeomType gt)
    : VectorFormVol<Scalar>(i, area), const_coeff(const_coeff), function_coeff(f_coeff), gt(gt)
  {
    // If f_coeff is HERMES_DEFAULT_FUNCTION, initialize it to be constant 1.0.
    if (f_coeff == HERMES_DEFAULT_FUNCTION) this->function_coeff = new DefaultFunction<Scalar>(1.0);
  }

  template<typename Scalar>
  DefaultVectorFormVol<Scalar>::DefaultVectorFormVol<Scalar>(int i, Hermes::vector<std::string> areas, Scalar const_coeff,
    DefaultFunction<Scalar>* f_coeff,
    GeomType gt)
    : VectorFormVol<Scalar>(i, areas), const_coeff(const_coeff), function_coeff(f_coeff), gt(gt)
  {
    // If f_coeff is HERMES_DEFAULT_FUNCTION, initialize it to be constant 1.0.
    if (f_coeff == HERMES_DEFAULT_FUNCTION) this->function_coeff = new DefaultFunction<Scalar>(1.0);
  }

  template<typename Scalar>
  DefaultVectorFormVol<Scalar>::~DefaultVectorFormVol<Scalar>() 
  {
    if (function_coeff != HERMES_DEFAULT_FUNCTION) delete function_coeff;
  };

  template<typename Scalar>
  Scalar DefaultVectorFormVol<Scalar>::value(int n, double *wt, Func<Scalar> *u_ext[], Func<double> *v,
    Geom<double> *e, ExtData<Scalar> *ext) const 
  {
    Scalar result = 0;
    if (gt == HERMES_PLANAR) 
    {
      for (int i = 0; i < n; i++) 
      {
        result += wt[i] * function_coeff->value(e->x[i], e->y[i]) * v->val[i];
      }
    }
    else 
    {
      if (gt == HERMES_AXISYM_X) 
      {
        for (int i = 0; i < n; i++) 
        {
          result += wt[i] * e->y[i] * function_coeff->value(e->x[i], e->y[i]) * v->val[i];
        }
      }
      else 
      {
        for (int i = 0; i < n; i++) 
        {
          result += wt[i] * e->x[i] * function_coeff->value(e->x[i], e->y[i]) * v->val[i];
        }
      }
    }
    return const_coeff * result;
  }

  template<typename Scalar>
  Ord DefaultVectorFormVol<Scalar>::ord(int n, double *wt, Func<Ord> *u_ext[], Func<Ord> *v,
    Geom<Ord> *e, ExtData<Ord> *ext) const 
  {
    Ord result = 0;
    if (gt == HERMES_PLANAR) 
    {
      for (int i = 0; i < n; i++) 
      {
        result += wt[i] * function_coeff->ord(e->x[i], e->y[i]) * v->val[i];
      }
    }
    else 
    {
      if (gt == HERMES_AXISYM_X) 
      {
        for (int i = 0; i < n; i++) 
        {
          result += wt[i] * e->y[i] * function_coeff->ord(e->x[i], e->y[i]) * v->val[i];
        }
      }
      else 
      {
        for (int i = 0; i < n; i++) 
        {
          result += wt[i] * e->x[i] * function_coeff->ord(e->x[i], e->y[i]) * v->val[i];
        }
      }
    }

    return result;
  }

  template<typename Scalar>
  VectorFormVol<Scalar>* DefaultVectorFormVol<Scalar>::clone() 
  {
    return new DefaultVectorFormVol<Scalar>(*this);
  }


  template<typename Scalar>
  DefaultResidualVol<Scalar>::DefaultResidualVol(int i, std::string area, Scalar const_coeff,
    DefaultFunction<Scalar>* f_coeff,
    GeomType gt)
    : VectorFormVol<Scalar>(i, area),
    idx_i(i), const_coeff(const_coeff), function_coeff(f_coeff), gt(gt)
  {
    // If f_coeff is HERMES_DEFAULT_FUNCTION, initialize it to be constant 1.0.
    if (f_coeff == HERMES_DEFAULT_FUNCTION) this->function_coeff = new DefaultFunction<Scalar>(1.0);
  }

  template<typename Scalar>
  DefaultResidualVol<Scalar>::DefaultResidualVol(int i, Hermes::vector<std::string> areas, Scalar const_coeff,
    DefaultFunction<Scalar>* f_coeff,
    GeomType gt)
    : VectorFormVol<Scalar>(i, areas),
    idx_i(i), const_coeff(const_coeff), function_coeff(f_coeff), gt(gt)
  {
    // If f_coeff is HERMES_DEFAULT_FUNCTION, initialize it to be constant 1.0.
    if (f_coeff == HERMES_DEFAULT_FUNCTION) this->function_coeff = new DefaultFunction<Scalar>(1.0);
  }

  template<typename Scalar>
  DefaultResidualVol<Scalar>::~DefaultResidualVol() 
  {
    if (function_coeff != HERMES_DEFAULT_FUNCTION) delete function_coeff;
  };

  template<typename Scalar>
  Scalar DefaultResidualVol<Scalar>::value(int n, double *wt, Func<Scalar> *u_ext[], Func<double> *v,
    Geom<double> *e, ExtData<Scalar> *ext) const 
  {
    Scalar result = 0;
    if (gt == HERMES_PLANAR) 
    {
      for (int i = 0; i < n; i++) 
      {
        result += wt[i] * function_coeff->value(e->x[i], e->y[i]) * u_ext[idx_i]->val[i] * v->val[i];
      }
    }
    else 
    {
      if (gt == HERMES_AXISYM_X) 
      {
        for (int i = 0; i < n; i++) 
        {
          result += wt[i] * e->y[i] * function_coeff->value(e->x[i], e->y[i]) * u_ext[idx_i]->val[i] * v->val[i];
        }
      }
      else 
      {
        for (int i = 0; i < n; i++) 
        {
          result += wt[i] * e->x[i] * function_coeff->value(e->x[i], e->y[i]) * u_ext[idx_i]->val[i] * v->val[i];
        }
      }
    }
    return const_coeff * result;
  }

  template<typename Scalar>
  Ord DefaultResidualVol<Scalar>::ord(int n, double *wt, Func<Ord> *u_ext[], Func<Ord> *v,
    Geom<Ord> *e, ExtData<Ord> *ext) const 
  {
    Ord result = 0;
    if (gt == HERMES_PLANAR) 
    {
      for (int i = 0; i < n; i++) 
      {
        result += wt[i] * function_coeff->ord(e->x[i], e->y[i]) * u_ext[idx_i]->val[i] * v->val[i];
      }
    }
    else 
    {
      if (gt == HERMES_AXISYM_X) 
      {
        for (int i = 0; i < n; i++) 
        {
          result += wt[i] * e->y[i] * function_coeff->ord(e->x[i], e->y[i]) * u_ext[idx_i]->val[i] * v->val[i];
        }
      }
      else 
      {
        for (int i = 0; i < n; i++) 
        {
          result += wt[i] * e->x[i] * function_coeff->ord(e->x[i], e->y[i]) * u_ext[idx_i]->val[i] * v->val[i];
        }
      }
    }

    return result;
  }

  template<typename Scalar>
  VectorFormVol<Scalar>* DefaultResidualVol<Scalar>::clone() 
  {
    return new DefaultResidualVol(*this);
  }


  template<typename Scalar>
  DefaultResidualDiffusion<Scalar>::DefaultResidualDiffusion(int i, std::string area, Scalar const_coeff,
    CubicSpline* c_spline,
    GeomType gt)
    : VectorFormVol<Scalar>(i, area),
    idx_i(i), const_coeff(const_coeff), spline_coeff(c_spline), gt(gt)
  {
    // If spline is HERMES_DEFAULT_SPLINE, initialize it to be constant 1.0.
    if (c_spline == HERMES_DEFAULT_SPLINE) this->spline_coeff = new CubicSpline(1.0);
  };

  template<typename Scalar>
  DefaultResidualDiffusion<Scalar>::DefaultResidualDiffusion(int i, Hermes::vector<std::string> areas, Scalar const_coeff,
    CubicSpline* c_spline, 
    GeomType gt)
    : VectorFormVol<Scalar>(i, areas),
    idx_i(i), const_coeff(const_coeff), spline_coeff(c_spline), gt(gt)
  {
    // If spline is HERMES_DEFAULT_SPLINE, initialize it to be constant 1.0.
    if (c_spline == HERMES_DEFAULT_SPLINE) this->spline_coeff = new CubicSpline(1.0);
  }

  template<typename Scalar>
  DefaultResidualDiffusion<Scalar>::~DefaultResidualDiffusion() 
  {
    if (spline_coeff != HERMES_DEFAULT_SPLINE) delete spline_coeff;
  };

  template<typename Scalar>
  Scalar DefaultResidualDiffusion<Scalar>::value(int n, double *wt, Func<Scalar> *u_ext[], Func<double> *v,
    Geom<double> *e, ExtData<Scalar> *ext) const
  {
    Scalar result = 0;
    if (gt == HERMES_PLANAR) 
    {
      for (int i = 0; i < n; i++) 
      {
        result += wt[i] * const_coeff * spline_coeff->get_value(u_ext[idx_i]->val[i])
          * (u_ext[idx_i]->dx[i] * v->dx[i] + u_ext[idx_i]->dy[i] * v->dy[i]);
      }
    }
    else 
    {
      if (gt == HERMES_AXISYM_X) 
      {
        for (int i = 0; i < n; i++) 
        {
          result += wt[i] * e->y[i] * const_coeff * spline_coeff->get_value(u_ext[idx_i]->val[i])
            * (u_ext[idx_i]->dx[i] * v->dx[i] + u_ext[idx_i]->dy[i] * v->dy[i]);
        }
      }
      else 
      {
        for (int i = 0; i < n; i++) 
        {
          result += wt[i] * e->x[i] * const_coeff * spline_coeff->get_value(u_ext[idx_i]->val[i])
            * (u_ext[idx_i]->dx[i] * v->dx[i] + u_ext[idx_i]->dy[i] * v->dy[i]);
        }
      }
    }

    return result;
  }

  template<typename Scalar>
  Ord DefaultResidualDiffusion<Scalar>::ord(int n, double *wt, Func<Ord> *u_ext[], Func<Ord> *v,
    Geom<Ord> *e, ExtData<Ord> *ext) const 
  {
    Ord result = 0;
    // Planar base.
    for (int i = 0; i < n; i++) 
    {
      result += wt[i] * const_coeff * spline_coeff->get_value(u_ext[idx_i]->val[i])
        * (u_ext[idx_i]->dx[i] * v->dx[i] + u_ext[idx_i]->dy[i] * v->dy[i]);
    }
    if (gt != HERMES_PLANAR) result = result * Ord(1);

    return result;
  }

  template<typename Scalar>
  VectorFormVol<Scalar>* DefaultResidualDiffusion<Scalar>::clone() 
  {
    return new DefaultResidualDiffusion(*this);
  }


  template<typename Scalar>
  DefaultResidualAdvection<Scalar>::DefaultResidualAdvection(int i, std::string area, 
    Scalar const_coeff1, Scalar const_coeff2, 
    CubicSpline* c_spline1,
    CubicSpline* c_spline2,
    GeomType gt)
    : VectorFormVol<Scalar>(i, area),
    idx_i(i), const_coeff1(const_coeff1), const_coeff2(const_coeff2), 
    spline_coeff1(c_spline1), spline_coeff2(c_spline2), gt(gt)
  {
    if (gt != HERMES_PLANAR) error("Axisymmetric advection forms not implemented yet.");

    // If spline1 == HERMES_DEFAULT_SPLINE or spline2 == HERMES_DEFAULT_SPLINE, initialize it to be constant 1.0.
    if (c_spline1 == HERMES_DEFAULT_SPLINE) this->spline_coeff1 = new CubicSpline(1.0);
    if (c_spline2 == HERMES_DEFAULT_SPLINE) this->spline_coeff2 = new CubicSpline(1.0);
  }

  template<typename Scalar>
  DefaultResidualAdvection<Scalar>::DefaultResidualAdvection(int i, Hermes::vector<std::string> areas,\
    Scalar const_coeff1, Scalar const_coeff2,
    CubicSpline* c_spline1,
    CubicSpline* c_spline2, GeomType gt)
    : VectorFormVol<Scalar>(i, areas),
    idx_i(i), const_coeff1(const_coeff1), const_coeff2(const_coeff2), 
    spline_coeff1(spline_coeff1), spline_coeff2(spline_coeff2), gt(gt)
  {
    if (gt != HERMES_PLANAR) error("Axisymmetric advection forms not implemented yet.");

    // If spline1 == HERMES_DEFAULT_SPLINE or spline2 == HERMES_DEFAULT_SPLINE, initialize it to be constant 1.0.
    if (c_spline1 == HERMES_DEFAULT_SPLINE) this->spline_coeff1 = new CubicSpline(1.0);
    if (c_spline2 == HERMES_DEFAULT_SPLINE) this->spline_coeff2 = new CubicSpline(1.0);
  }

  template<typename Scalar>
  DefaultResidualAdvection<Scalar>::~DefaultResidualAdvection() 
  {
    if (spline_coeff1 != HERMES_DEFAULT_SPLINE) delete spline_coeff1;
    if (spline_coeff2 != HERMES_DEFAULT_SPLINE) delete spline_coeff2;
  };

  template<typename Scalar>
  Scalar DefaultResidualAdvection<Scalar>::value(int n, double *wt, Func<Scalar> *u_ext[], Func<double> *v,
    Geom<double> *e, ExtData<Scalar> *ext) const 
  {
    Scalar result = 0;
    Func<Scalar>* u_prev = u_ext[idx_i];
    for (int i = 0; i < n; i++) 
    {
      result += wt[i] * (const_coeff1*spline_coeff1->get_value(u_prev->val[i]) * (u_prev->dx[i] * v->val[i])
        + const_coeff2*spline_coeff2->get_value(u_prev->val[i]) * (u_prev->dy[i] * v->val[i]));
    }
    return result;
  }

  template<typename Scalar>
  Ord DefaultResidualAdvection<Scalar>::ord(int n, double *wt, Func<Ord> *u_ext[], Func<Ord> *v,
    Geom<Ord> *e, ExtData<Ord> *ext) const 
  {
    Ord result = 0;
    Func<Scalar>* u_prev = u_ext[idx_i];
    for (int i = 0; i < n; i++) 
    {
      result += wt[i] * (const_coeff1*spline_coeff1->get_value(u_prev->val[i]) * (u_prev->dx[i] * v->val[i])
        + const_coeff2*spline_coeff2->get_value(u_prev->val[i]) * (u_prev->dy[i] * v->val[i]));
    }
    return result;
  }

  template<typename Scalar>
  VectorFormVol<Scalar>* DefaultResidualAdvection<Scalar>::clone() 
  {
    return new DefaultResidualAdvection(*this);
  }


  template<typename Scalar>
  DefaultMatrixFormSurf<Scalar>::DefaultMatrixFormSurf<Scalar>(int i, int j, std::string area,
    Scalar const_coeff, DefaultFunction<Scalar>* f_coeff,
    GeomType gt)
    : MatrixFormSurf<Scalar>(i, j, area), const_coeff(const_coeff), function_coeff(f_coeff), gt(gt)
  {
    // If f_coeff is HERMES_DEFAULT_FUNCTION, initialize it to be constant 1.0.
    if (f_coeff == HERMES_DEFAULT_FUNCTION) this->function_coeff = new DefaultFunction<Scalar>(1.0);
  }

  template<typename Scalar>
  DefaultMatrixFormSurf<Scalar>::DefaultMatrixFormSurf<Scalar>(int i, int j, Hermes::vector<std::string> areas,
    Scalar const_coeff, DefaultFunction<Scalar>* f_coeff,
    GeomType gt)
    : MatrixFormSurf<Scalar>(i, j, areas), const_coeff(const_coeff), function_coeff(f_coeff), gt(gt)
  {
    // If f_coeff is HERMES_DEFAULT_FUNCTION, initialize it to be constant 1.0.
    if (f_coeff == HERMES_DEFAULT_FUNCTION) this->function_coeff = new DefaultFunction<Scalar>(1.0);
  }

  template<typename Scalar>
  DefaultMatrixFormSurf<Scalar>::~DefaultMatrixFormSurf<Scalar>() 
  {
    if (function_coeff != HERMES_DEFAULT_FUNCTION) delete function_coeff;
  };

  template<typename Scalar>
  Scalar DefaultMatrixFormSurf<Scalar>::value(int n, double *wt, Func<Scalar> *u_ext[], Func<double> *u, Func<double> *v,
    Geom<double> *e, ExtData<Scalar> *ext) const 
  {
    Scalar result = 0;
    if (gt == HERMES_PLANAR) 
    {
      for (int i = 0; i < n; i++) 
      {
        result += wt[i] * function_coeff->value(e->x[i], e->y[i]) * u->val[i] * v->val[i];
      }
    }
    else 
    {
      if (gt == HERMES_AXISYM_X) 
      {
        for (int i = 0; i < n; i++) 
        {
          result += wt[i] * e->y[i] * function_coeff->value(e->x[i], e->y[i]) * u->val[i] * v->val[i];
        }
      }
      else 
      {
        for (int i = 0; i < n; i++) 
        {
          result += wt[i] * e->x[i] * function_coeff->value(e->x[i], e->y[i]) * u->val[i] * v->val[i];
        }
      }
    }

    return const_coeff * result;
  }

  template<typename Scalar>
  Ord DefaultMatrixFormSurf<Scalar>::ord(int n, double *wt, Func<Ord> *u_ext[], Func<Ord> *u,
    Func<Ord> *v, Geom<Ord> *e, ExtData<Ord> *ext) const 
  {
    Ord result = 0;
    if (gt == HERMES_PLANAR) 
    {
      for (int i = 0; i < n; i++) 
      {
        result += wt[i] * function_coeff->ord(e->x[i], e->y[i]) * u->val[i] * v->val[i];
      }
    }
    else 
    {
      if (gt == HERMES_AXISYM_X) 
      {
        for (int i = 0; i < n; i++) 
        {
          result += wt[i] * e->y[i] * function_coeff->ord(e->x[i], e->y[i]) * u->val[i] * v->val[i];
        }
      }
      else 
      {
        for (int i = 0; i < n; i++) 
        {
          result += wt[i] * e->x[i] * function_coeff->ord(e->x[i], e->y[i]) * u->val[i] * v->val[i];
        }
      }
    }

    return result;
  }

  template<typename Scalar>
  MatrixFormSurf<Scalar>* DefaultMatrixFormSurf<Scalar>::clone() 
  {
    return new DefaultMatrixFormSurf<Scalar>(*this);
  }


  template<typename Scalar>
  DefaultJacobianFormSurf<Scalar>::DefaultJacobianFormSurf<Scalar>(int i, int j, std::string area, Scalar const_coeff,
    CubicSpline* c_spline,
    GeomType gt)
    : MatrixFormSurf<Scalar>(i, j, area),
    idx_j(j), const_coeff(const_coeff), spline_coeff(c_spline), gt(gt)
  {
    // If spline is HERMES_DEFAULT_SPLINE, initialize it to be constant 1.0.
    if (c_spline == HERMES_DEFAULT_SPLINE) this->spline_coeff = new CubicSpline(1.0);
  }

  template<typename Scalar>
  DefaultJacobianFormSurf<Scalar>::DefaultJacobianFormSurf<Scalar>(int i, int j, Hermes::vector<std::string> areas, Scalar const_coeff,
    CubicSpline* c_spline,
    GeomType gt)
    : MatrixFormSurf<Scalar>(i, j, areas), const_coeff(const_coeff), spline_coeff(spline_coeff), gt(gt)
  {
    // If spline is HERMES_DEFAULT_SPLINE, initialize it to be constant 1.0.
    if (c_spline == HERMES_DEFAULT_SPLINE) this->spline_coeff = new CubicSpline(1.0);
  }

  template<typename Scalar>
  DefaultJacobianFormSurf<Scalar>::~DefaultJacobianFormSurf<Scalar>() 
  {
    if (spline_coeff != HERMES_DEFAULT_SPLINE) delete spline_coeff;
  };

  template<typename Scalar>
  Scalar DefaultJacobianFormSurf<Scalar>::value(int n, double *wt, Func<Scalar> *u_ext[], Func<double> *u, Func<double> *v,
    Geom<double> *e, ExtData<Scalar> *ext) const 
  {
    Scalar result = 0;
    for (int i = 0; i < n; i++) 
    {
      result += wt[i] * (const_coeff*spline_coeff->get_derivative(u_ext[idx_j]->val[i]) * u_ext[idx_j]->val[i]
      + const_coeff*spline_coeff->get_value(u_ext[idx_j]->val[i]))
        * u->val[i] * v->val[i];
    }
    return result;
  }

  template<typename Scalar>
  Ord DefaultJacobianFormSurf<Scalar>::ord(int n, double *wt, Func<Ord> *u_ext[], Func<Ord> *u,
    Func<Ord> *v, Geom<Ord> *e, ExtData<Ord> *ext) const 
  {
    Ord result = 0;
    for (int i = 0; i < n; i++) 
    {
      result += wt[i] * (const_coeff*spline_coeff->get_derivative(u_ext[idx_j]->val[i]) * u_ext[idx_j]->val[i]
      + const_coeff*spline_coeff->get_value(u_ext[idx_j]->val[i]))
        * u->val[i] * v->val[i];
    }
    return result;
  }

  template<typename Scalar>
  MatrixFormSurf<Scalar>* DefaultJacobianFormSurf<Scalar>::clone() 
  {
    return new DefaultJacobianFormSurf<Scalar>(*this);
  }


  template<typename Scalar>
  DefaultVectorFormSurf<Scalar>::DefaultVectorFormSurf<Scalar>(int i, std::string area, Scalar const_coeff,
    DefaultFunction<Scalar>* f_coeff,
    GeomType gt)
    : VectorFormSurf<Scalar>(i, area), const_coeff(const_coeff), function_coeff(f_coeff), gt(gt)
  {
    // If f_coeff is HERMES_DEFAULT_FUNCTION, initialize it to be constant 1.0.
    if (f_coeff == HERMES_DEFAULT_FUNCTION) this->function_coeff = new DefaultFunction<Scalar>(1.0);
  }

  template<typename Scalar>
  DefaultVectorFormSurf<Scalar>::DefaultVectorFormSurf<Scalar>(int i, Hermes::vector<std::string> areas, Scalar const_coeff,
    DefaultFunction<Scalar>* f_coeff,
    GeomType gt)
    : VectorFormSurf<Scalar>(i, areas), const_coeff(const_coeff), function_coeff(f_coeff), gt(gt)
  {
    // If f_coeff is HERMES_DEFAULT_FUNCTION, initialize it to be constant 1.0.
    if (f_coeff == HERMES_DEFAULT_FUNCTION) this->function_coeff = new DefaultFunction<Scalar>(1.0);
  }

  template<typename Scalar>
  DefaultVectorFormSurf<Scalar>::~DefaultVectorFormSurf<Scalar>() 
  {
    if (function_coeff != HERMES_DEFAULT_FUNCTION) delete function_coeff;
  };

  template<typename Scalar>
  Scalar DefaultVectorFormSurf<Scalar>::value(int n, double *wt, Func<Scalar> *u_ext[], Func<double> *v,
    Geom<double> *e, ExtData<Scalar> *ext) const 
  {
    Scalar result = 0;
    if (gt == HERMES_PLANAR) 
    {
      for (int i = 0; i < n; i++) 
      {
        result += wt[i] * function_coeff->value(e->x[i], e->y[i]) * v->val[i];
      }
    }
    else 
    {
      if (gt == HERMES_AXISYM_X) 
      {
        for (int i = 0; i < n; i++) 
        {
          result += wt[i] * e->y[i] * function_coeff->value(e->x[i], e->y[i]) * v->val[i];
        }
      }
      else 
      {
        for (int i = 0; i < n; i++) 
        {
          result += wt[i] * e->x[i] * function_coeff->value(e->x[i], e->y[i]) * v->val[i];
        }
      }
    }

    return const_coeff * result;
  }

  template<typename Scalar>
  Ord DefaultVectorFormSurf<Scalar>::ord(int n, double *wt, Func<Ord> *u_ext[], Func<Ord> *v,
    Geom<Ord> *e, ExtData<Ord> *ext) const 
  {
    Ord result = 0;
    if (gt == HERMES_PLANAR) 
    {
      for (int i = 0; i < n; i++) 
      {
        result += wt[i] * function_coeff->ord(e->x[i], e->y[i]) * v->val[i];
      }
    }
    else 
    {
      if (gt == HERMES_AXISYM_X) 
      {
        for (int i = 0; i < n; i++) 
        {
          result += wt[i] * e->y[i] * function_coeff->ord(e->x[i], e->y[i]) * v->val[i];
        }
      }
      else 
      {
        for (int i = 0; i < n; i++) 
        {
          result += wt[i] * e->x[i] * function_coeff->ord(e->x[i], e->y[i]) * v->val[i];
        }
      }
    }

    return result;
  }

  template<typename Scalar>
  VectorFormSurf<Scalar>* DefaultVectorFormSurf<Scalar>::clone() 
  {
    return new DefaultVectorFormSurf<Scalar>(*this);
  }


  template<typename Scalar>
  DefaultMultiComponentVectorFormSurf<Scalar>::DefaultMultiComponentVectorFormSurf<Scalar>(Hermes::vector<unsigned int> coordinates,
    std::string area,
    Hermes::vector<Scalar> coeffs,
    GeomType gt)
    : WeakForm::MultiComponentVectorFormSurf<Scalar>(coordinates, area), coeffs(coeffs), gt(gt) 
  {
  }

  template<typename Scalar>
  DefaultMultiComponentVectorFormSurf<Scalar>::DefaultMultiComponentVectorFormSurf<Scalar>(Hermes::vector<unsigned int> coordinates,
    Hermes::vector<std::string> areas,
    Hermes::vector<Scalar> coeffs, GeomType gt)
    : WeakForm::MultiComponentVectorFormSurf<Scalar>(coordinates, areas), coeffs(coeffs), gt(gt) 
  {
  }

  template<typename Scalar>
  void DefaultMultiComponentVectorFormSurf<Scalar>::value(int n, double *wt, Func<Scalar> *u_ext[], Func<double> *v,
    Geom<double> *e, ExtData<Scalar> *ext, 
    Hermes::vector<Scalar>& result) const 
  {
    Scalar result_base = 0;
    if (gt == HERMES_PLANAR)
      result_base = int_v<double>(n, wt, v);
    else
      if (gt == HERMES_AXISYM_X)
        result_base = int_y_v<double>(n, wt, v, e);
      else
        result_base = int_x_v<double>(n, wt, v, e);

    for(unsigned int result_i = 0; result_i < this->coordinates.size(); result_i++)
      result.push_back(result_base * coeffs[result_i]);
  }

  template<typename Scalar>
  Ord DefaultMultiComponentVectorFormSurf<Scalar>::ord(int n, double *wt, Func<Ord> *u_ext[], Func<Ord> *v,
    Geom<Ord> *e, ExtData<Ord> *ext) const 
  {
    if (gt == HERMES_PLANAR)
      return int_v<Ord>(n, wt, v);
    else
      if (gt == HERMES_AXISYM_X)
        return int_y_v<Ord>(n, wt, v, e);
      else
        return int_x_v<Ord>(n, wt, v, e);
  }

  template<typename Scalar>
  DefaultResidualSurf<Scalar>::DefaultResidualSurf(int i, std::string area, Scalar const_coeff,
    DefaultFunction<Scalar>* f_coeff,
    GeomType gt)
    : VectorFormSurf<Scalar>(i, area),
    idx_i(i), const_coeff(const_coeff), function_coeff(f_coeff), gt(gt)
  {
    // If f_coeff is HERMES_DEFAULT_FUNCTION, initialize it to be constant 1.0.
    if (f_coeff == HERMES_DEFAULT_FUNCTION) this->function_coeff = new DefaultFunction<Scalar>(1.0);
  }

  template<typename Scalar>
  DefaultResidualSurf<Scalar>::DefaultResidualSurf(int i, Hermes::vector<std::string> areas, Scalar const_coeff,
    DefaultFunction<Scalar>* f_coeff,
    GeomType gt)
    : VectorFormSurf<Scalar>(i, areas),
    idx_i(i), const_coeff(const_coeff), function_coeff(f_coeff), gt(gt)
  {
    // If f_coeff is HERMES_DEFAULT_FUNCTION, initialize it to be constant 1.0.
    if (f_coeff == HERMES_DEFAULT_FUNCTION) this->function_coeff = new DefaultFunction<Scalar>(1.0);
  }

  template<typename Scalar>
  DefaultResidualSurf<Scalar>::~DefaultResidualSurf() 
  {
    if (function_coeff != HERMES_DEFAULT_FUNCTION) delete function_coeff;
  };

  template<typename Scalar>
  Scalar DefaultResidualSurf<Scalar>::value(int n, double *wt, Func<Scalar> *u_ext[], Func<double> *v,
    Geom<double> *e, ExtData<Scalar> *ext) const 
  {
    Scalar result = 0;
    if (gt == HERMES_PLANAR) 
    {
      for (int i = 0; i < n; i++) 
      {
        result += wt[i] * function_coeff->value(e->x[i], e->y[i]) * u_ext[idx_i]->val[i] * v->val[i];
      }
    }
    else 
    {
      if (gt == HERMES_AXISYM_X) 
      {
        for (int i = 0; i < n; i++) 
        {
          result += wt[i] * e->y[i] * function_coeff->value(e->x[i], e->y[i]) * u_ext[idx_i]->val[i] * v->val[i];
        }
      }
      else 
      {
        for (int i = 0; i < n; i++) 
        {
          result += wt[i] * e->x[i] * function_coeff->value(e->x[i], e->y[i]) * u_ext[idx_i]->val[i] * v->val[i];
        }
      }
    }

    return const_coeff * result;
  }

  template<typename Scalar>
  Ord DefaultResidualSurf<Scalar>::ord(int n, double *wt, Func<Ord> *u_ext[], Func<Ord> *v,
    Geom<Ord> *e, ExtData<Ord> *ext) const 
  {
    Ord result = 0;
    if (gt == HERMES_PLANAR) 
    {
      for (int i = 0; i < n; i++) 
      {
        result += wt[i] * function_coeff->ord(e->x[i], e->y[i]) * u_ext[idx_i]->val[i] * v->val[i];
      }
    }
    else 
    {
      if (gt == HERMES_AXISYM_X) 
      {
        for (int i = 0; i < n; i++) 
        {
          result += wt[i] * e->y[i] * function_coeff->ord(e->x[i], e->y[i]) * u_ext[idx_i]->val[i] * v->val[i];
        }
      }
      else 
      {
        for (int i = 0; i < n; i++) 
        {
          result += wt[i] * e->x[i] * function_coeff->ord(e->x[i], e->y[i]) * u_ext[idx_i]->val[i] * v->val[i];
        }
      }
    }

    return result;
  }

  template<typename Scalar>
  VectorFormSurf<Scalar>* DefaultResidualSurf<Scalar>::clone() 
  {
    return new DefaultResidualSurf(*this);
  }


  template<typename Scalar>
  DefaultWeakFormLaplace<Scalar>::DefaultWeakFormLaplace(std::string area, Scalar const_coeff,
    CubicSpline* spline_coeff,
    GeomType gt) : WeakForm()
  {
    // Jacobian.
    add_matrix_form(new DefaultJacobianDiffusion(0, 0, area, const_coeff,
      spline_coeff, HERMES_SYM, gt));
    // Residual.
    add_vector_form(new DefaultResidualDiffusion(0, area, const_coeff,
      spline_coeff, gt));
  };


  template<typename Scalar>
  DefaultWeakFormPoisson<Scalar>::DefaultWeakFormPoisson(DefaultFunction<Scalar>* rhs,
    std::string area, Scalar const_coeff,
    CubicSpline* spline_coeff,
    GeomType gt) : WeakForm()
  {
    // Jacobian.
    add_matrix_form(new DefaultJacobianDiffusion(0, 0, area, const_coeff,
      spline_coeff, HERMES_NONSYM, gt));
    // Residual.
    add_vector_form(new DefaultResidualDiffusion(0, area, const_coeff,
      spline_coeff, gt));
    add_vector_form(new DefaultVectorFormVol<Scalar>(0, area, -1.0, rhs, gt));
  };
};
