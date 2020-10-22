/*!
 * \file CTableFluid.cpp
 * \brief Source of the interpolation table fluid model.
 * \author F. Dittmann
 * \version 7.0.7 "Blackbird"
 *
 * SU2 Project Website: https://su2code.github.io
 *
 * The SU2 Project is maintained by the SU2 Foundation
 * (http://su2foundation.org)
 *
 * Copyright 2012-2020, SU2 Contributors (cf. AUTHORS.md)
 *
 * SU2 is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * SU2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with SU2. If not, see <http://www.gnu.org/licenses/>.
 */

#include "../../include/fluid/CTableFluid.hpp"

CTableFluid::CTableFluid(bool CompEntropy) : CFluidModel() {
  ComputeEntropy = CompEntropy;
}

//saturation energy 
su2double CTableFluid::esat_rho(su2double rho) const
{   
    return coefesat[0] + coefesat[1]*rho + coefesat[2]*pow(rho,ONE2) + coefesat[3]*pow(rho,ONE3);
}

//interpolation
su2double CTableFluid::interpolateTable(su2double xi, su2double yi, const su2double x[2], const su2double y[2], const su2double z[Nx*Ny]) const
{   
    //fractional indices assuming equal spacing
    su2double ix = (xi-x[0])/(x[1]-x[0]) *(Nx-1); su2double iy = (yi-y[0])/(y[1]-y[0]) *(Ny-1);
    //bounded left and right indices (out of bound indices will result in extrapolation)
    int ixl = min(max(0,(int) SU2_TYPE::GetValue(ix)), Nx-2); int ixr = ixl+1;
    int iyl = min(max(0,(int) SU2_TYPE::GetValue(iy)), Ny-2); int iyr = iyl+1;      
    //linear interpolation
    su2double zil = z[ixl*Ny+iyl] + (iy - (su2double) iyl) *(z[ixl*Ny+iyr] - z[ixl*Ny+iyl]);
    su2double zir = z[ixr*Ny+iyl] + (iy - (su2double) iyl) *(z[ixr*Ny+iyr] - z[ixr*Ny+iyl]);
    return zil + (ix - (su2double) ixl) *(zir - zil);
}

//root finding
su2double CTableFluid::rootFunc(su2double x0, function<su2double(su2double)>func) const
{
    //initial guess
    int n = 0;
    su2double x = x0;        
    su2double y = func(x);
    x0  = 1.01*x0;
    su2double y0, dy, dx;
    //secant method
    while (abs(y)>1e-9*x0 && n<20) 
    {
        y0 = func(x0);
	    dy = y - y0;
	    dx = x - x0;
	    y = y0;
   	 	x = x0; 
	    x0 = x0 - y0*dx/dy;
        n++;
    }
    return x;
}


//---state variables from interpolation of rho-e table---

su2double CTableFluid::P_rhoe(su2double rhoi, su2double ei) const
{
    su2double Dei = ei - esat_rho(rhoi);
    return interpolateTable(rhoi,Dei,Rho,De,P_rhoDe);
}

su2double CTableFluid::T_rhoe(su2double rhoi, su2double ei) const
{
    su2double Dei = ei - esat_rho(rhoi);
    return interpolateTable(rhoi,Dei,Rho,De,T_rhoDe);
}

su2double CTableFluid::h_rhoe(su2double rhoi, su2double ei) const
{
    su2double Dei = ei - esat_rho(rhoi);
    return interpolateTable(rhoi,Dei,Rho,De,h_rhoDe);
}

su2double CTableFluid::s_rhoe(su2double rhoi, su2double ei) const
{
    su2double Dei = ei - esat_rho(rhoi);
    return interpolateTable(rhoi,Dei,Rho,De,s_rhoDe);
}

su2double CTableFluid::cv_rhoe(su2double rhoi, su2double ei) const
{
    su2double Dei = ei - esat_rho(rhoi);
    return interpolateTable(rhoi,Dei,Rho,De,cv_rhoDe);
}

su2double CTableFluid::cp_rhoe(su2double rhoi, su2double ei) const
{
    su2double Dei = ei - esat_rho(rhoi);
    return interpolateTable(rhoi,Dei,Rho,De,cp_rhoDe);
}

su2double CTableFluid::a2_rhoe(su2double rhoi, su2double ei) const
{
    su2double Dei = ei - esat_rho(rhoi);
    return interpolateTable(rhoi,Dei,Rho,De,a2_rhoDe);
}

su2double CTableFluid::dPdrho_e_rhoe(su2double rhoi, su2double ei) const
{
    su2double Dei = ei - esat_rho(rhoi);
    return interpolateTable(rhoi,Dei,Rho,De,dPdrho_e_rhoDe);
}

su2double CTableFluid::dPde_rho_rhoe(su2double rhoi, su2double ei) const
{
    su2double Dei = ei - esat_rho(rhoi);
    return interpolateTable(rhoi,Dei,Rho,De,dPde_rho_rhoDe);
}

su2double CTableFluid::dTdrho_e_rhoe(su2double rhoi, su2double ei) const
{
    su2double Dei = ei - esat_rho(rhoi);
    return interpolateTable(rhoi,Dei,Rho,De,dTdrho_e_rhoDe);
}

su2double CTableFluid::dTde_rho_rhoe(su2double rhoi, su2double ei) const
{
    su2double Dei = ei - esat_rho(rhoi);
    return interpolateTable(rhoi,Dei,Rho,De,dTde_rho_rhoDe);
}


//---state variables from root finding of interpolation of rho-e table--

su2double CTableFluid::e_rhoP(su2double rhoi, su2double Pi) const
{
    su2double ei0 = esat_rho(rhoi) + De[0];
    return rootFunc(ei0,[this,rhoi,Pi](su2double x){return P_rhoe(rhoi,x)-Pi;});
}

su2double CTableFluid::e_rhoT(su2double rhoi, su2double Ti) const
{
    su2double ei0 = esat_rho(rhoi) + De[0];
    return rootFunc(ei0,[this,rhoi,Ti](su2double x){return T_rhoe(rhoi,x)-Ti;});
}

su2double CTableFluid::e_rhoh(su2double rhoi, su2double hi) const
{
    su2double ei0 = esat_rho(rhoi) + De[0];
    return rootFunc(ei0,[this,rhoi,hi](su2double x){return h_rhoe(rhoi,x)-hi;});
}


//---state variables from root finding of root finding of interpolation of rho-e table---

su2double CTableFluid::rho_PT(su2double Pi, su2double Ti) const
{
    su2double rhoi0 = Rho[0];
    return rootFunc(rhoi0,[this,Pi,Ti](su2double x){return T_rhoe(x,e_rhoP(x,Pi))-Ti;});
}

su2double CTableFluid::rho_Ps(su2double Pi, su2double si) const
{
    su2double rhoi0 = Rho[0];
    return rootFunc(rhoi0,[this,Pi,si](su2double x){return s_rhoe(x,e_rhoP(x,Pi))-si;});
}

su2double CTableFluid::rho_hs(su2double hi, su2double si) const
{
    su2double rhoi0 = Rho[0];
    return rootFunc(rhoi0,[this,hi,si](su2double x){return s_rhoe(x,e_rhoh(x,hi))-si;});
}


//---cheap set state call---

void CTableFluid::SetTDState_rhoe(su2double rho, su2double e)
{
    Density = rho;
    StaticEnergy = e;
    Pressure = P_rhoe(rho,e);
    Temperature = T_rhoe(rho,e);
    //StaticEnthalpy = h_rhoe(rho,e);
    
    SoundSpeed2 = a2_rhoe(rho,e);
    dPdrho_e = dPdrho_e_rhoe(rho,e);
    dPde_rho = dPde_rho_rhoe(rho,e);
    dTdrho_e = dTdrho_e_rhoe(rho,e);
    dTde_rho = dTde_rho_rhoe(rho,e);
    
    Cv = cv_rhoe(rho,e);
    Cp = cp_rhoe(rho,e);
    
    if (ComputeEntropy) Entropy = s_rhoe(rho,e);
}


//---not so cheap set state calls---

void CTableFluid::SetEnergy_Prho(su2double P, su2double rho)
{
    StaticEnergy = e_rhoP(rho,P);
}

void CTableFluid::SetTDState_Prho(su2double P, su2double rho)
{
    SetTDState_rhoe(rho,e_rhoP(rho,P));
}

void CTableFluid::SetTDState_rhoT(su2double rho, su2double T)
{   
    SetTDState_rhoe(rho,e_rhoT(rho,T));
}

void CTableFluid::SetTDState_rhoh(su2double rho, su2double h)
{   
    SetTDState_rhoe(rho,e_rhoh(rho,h));
}


//---expensive set state calls---

void CTableFluid::SetTDState_PT(su2double P, su2double T)
{
    SetTDState_Prho(P,rho_PT(P,T));   
}

void CTableFluid::SetTDState_Ps(su2double P, su2double s)
{
    SetTDState_Prho(P,rho_Ps(P,s));    
}

void CTableFluid::SetTDState_hs(su2double h, su2double s)
{
    SetTDState_rhoh(rho_hs(h,s),h);    
}
