/*!
 * \file geometry_structure.hpp
 * \brief Headers of the main subroutines for creating the geometrical structure.
 *        The subroutines and functions are in the <i>geometry_structure.cpp</i> file.
 * \author Current Development: Stanford University.
 *         Original Structure: CADES 1.0 (2009).
 * \version 1.0.
 *
 * Stanford University Unstructured (SU2) Code
 * Copyright (C) 2012 Aerospace Design Laboratory
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef NO_METIS
extern "C" {
#include "metis.h"
}
#endif

#ifndef NO_MPI
#include <mpi.h>
#endif

#ifndef NO_CGNS
#include "cgnslib.h"
#endif

#include "primal_grid_structure.hpp"
#include "dual_grid_structure.hpp"
#include "config_structure.hpp"

using namespace std;

/*! 
 * \class CGeometry
 * \brief Parent class for defining the geometry of the problem (complete geometry, 
 *        multigrid agglomerated geometry, only boundary geometry, etc..)
 * \author F. Palacios.
 * \version 1.0.
 */
class CGeometry {
protected:
	unsigned long nPoint,	/*!< \brief Number of points of the mesh. */
	nPointDomain,						/*!< \brief Number of real points of the mesh. */
	nPointGhost,					/*!< \brief Number of ghost points of the mesh. */
	nElem,					/*!< \brief Number of elements of the mesh. */
	nEdge,					/*!< \brief Number of edges of the mesh. */
	nElem_Storage;			/*!< \brief Storage capacity for ParaView format (domain). */ 
	unsigned short nDim,	/*!< \brief Number of dimension of the problem. */
	nMarker;				/*!< \brief Number of diferent markers of the mesh. */

public:
	unsigned long *nElem_Bound_Storage;	/*!< \brief Storage capacity for ParaView format (boundaries, for each marker). */ 
	unsigned long *nElem_Bound;			/*!< \brief Number of elements of the boundary. */
	string *Tag_to_Marker;	/*!< \brief If you know the index of the boundary (depend of the 
							 grid definition), it gives you the maker (where the boundary 
							 is stored from 0 to boundaries). */	
	CPrimalGrid** elem;	/*!< \brief Element vector (primal grid information). */
	CPrimalGrid*** bound;	/*!< \brief Boundary vector (primal grid information). */
	CPoint** node;			/*!< \brief Node vector (dual grid information). */
	CEdge** edge;			/*!< \brief Edge vector (dual grid information). */
	CVertex*** vertex;		/*!< \brief Boundary Vertex vector (dual grid information). */
	unsigned long *nVertex;	/*!< \brief Number of vertex for each marker. */
	vector<unsigned long> SendDomain[MAX_NUMBER_DOMAIN][MAX_NUMBER_DOMAIN];	/*!< \brief SendDomain[from domain][to domain] and return the 
																			 point index of the node that must me sended. */
	vector<unsigned long> SendTransf[MAX_NUMBER_DOMAIN][MAX_NUMBER_DOMAIN];	/*!< \brief Vector to store the type of transformation for this
																			 send point. */
	vector<unsigned long> PeriodicPoint[MAX_NUMBER_PERIODIC][2];			/*!< \brief PeriodicPoint[Periodic bc] and return the point that 
																			 must be sended [0], and the image point in the periodic bc[1]. */
	vector<unsigned long> PeriodicElem[MAX_NUMBER_PERIODIC];				/*!< \brief PeriodicElem[Periodic bc] and return the elements that 
																			 must be sended. */
	vector<unsigned long> NewBoundaryPoints[MAX_NUMBER_MARKER];  /*!< \brief Vector containing new points appearing on multiple boundaries. */
	vector<unsigned long> OldBoundaryElems[MAX_NUMBER_MARKER];  /*!< \brief Vector of old boundary elements. */
	CPrimalGrid*** newBound;            /*!< \brief Boundary vector for new periodic elements (primal grid information). */
	unsigned long *nNewElem_Bound;			/*!< \brief Number of new periodic elements of the boundary. */
	long *PeriodicDomainIndex;
	
	/*! 
	 * \brief Constructor of the class.
	 */
	CGeometry(void);
	
	/*! 
	 * \brief Destructor of the class.
	 */
	~CGeometry(void);
	
	/*! 
	 * \brief Set the right grid from Lockheed project.
	 */
	virtual void SetLockheedGrid(CConfig *config);
	
	/*! 
	 * \brief Get number of coordinates.
	 * \return Number of coordinates.
	 */
	unsigned short GetnDim(void);
	
	/*! 
	 * \brief Get number of points.
	 * \return Number of points.
	 */
	unsigned long GetnPoint(void);
	
	/*! 
	 * \brief Get number of real points (that belong to the domain).
	 * \return Number of real points.
	 */
	unsigned long GetnPointDomain(void);
	
	/*! 
	 * \brief Get number of elements.
	 * \return Number of elements.
	 */
	unsigned long GetnElem(void);
	
	/*! 
	 * \brief Get number of edges.
	 * \return Number of edges.
	 */
	unsigned long GetnEdge(void);
	
	/*! 
	 * \brief Get number of markers.
	 * \return Number of markers.
	 */
	unsigned short GetnMarker(void);
	
	/*! 
	 * \brief Get number of vertices.
	 * \param[in] val_marker - Marker of the boundary.
	 * \return Number of vertices.
	 */
	unsigned long GetnVertex(unsigned short val_marker);
		
	/*! 
	 * \brief Get the edge index from using the nodes of the edge.
	 * \param[in] first_point - First point of the edge.
	 * \param[in] second_point - Second point of the edge.
	 * \return Index of the edge.
	 */		
	long FindEdge(unsigned long first_point, unsigned long second_point);
	
	/*! 
	 * \brief Create a file for testing the geometry.
	 */		
	void TestGeometry(void);
	
	/*! 
	 * \brief A virtual member.
	 * \param[in] val_nmarker - Number of markers.
	 */
	void SetnMarker(unsigned short val_nmarker);
	
	/*! 
	 * \brief Set the number of dimensions of the problem.
	 * \param[in] val_ndim - Number of dimensions.
	 */
	void SetnDim(unsigned short val_ndim);
	
	/*! 
	 * \brief Get the index of a marker.
	 * \param[in] val_marker - Marker of the boundary.
	 * \return Index of the marker in the grid defintion.
	 */	
	string GetMarker_Tag(unsigned short val_marker);
	
	/*! 
	 * \brief Set index of a marker.
	 * \param[in] val_marker - Marker of the boundary.
	 * \param[in] val_index - Index of the marker.
	 */		
	void SetMarker_Tag(unsigned short val_marker, string val_index);
	
	/*! 
	 * \brief Set the number of boundary elements.
	 * \param[in] val_marker - Marker of the boundary.
	 * \param[in] val_nelem_bound - Number of boundary elements.
	 */	
	void SetnElem_Bound(unsigned short val_marker, unsigned long val_nelem_bound);
	
	/*! 
	 * \brief Set the number of storage for boundary elements.
	 * \param[in] val_marker - Marker of the boundary.
	 * \param[in] val_nelem_bound - Number of boundary elements.
	 */	
	void SetnElem_Bound_Storage(unsigned short val_marker, unsigned long val_nelem_bound);
	
	/*! 
	 * \brief Set the number of grid points.
	 * \param[in] val_npoint - Number of grid points.
	 */	
	void SetnPoint(unsigned long val_npoint);
	
	/*! 
	 * \brief Set the number of grid elements.
	 * \param[in] val_nelem - Number of grid elements.
	 */
	void SetnElem(unsigned long val_nelem);
	
	/*! 
	 * \brief Get the number of boundary elements.
	 * \param[in] val_marker - Marker of the boundary.
	 */
	unsigned long GetnElem_Bound(unsigned short val_marker);
	
	/*! 
	 * \brief Get the number of storage boundary elements.
	 * \param[in] val_marker - Marker of the boundary.
	 */
	unsigned long GetnElem_Bound_Storage(unsigned short val_marker);
	
	/*! 
	 * \brief Set the number of elements in vtk fortmat.
	 * \param[in] val_nelem_storage - Number of elements
	 */
	void SetnElem_Storage(unsigned long val_nelem_storage);
	
	/*! 
	 * \brief Get the number of elements in vtk fortmat.
	 */	
	unsigned long GetnElem_Storage(void);
	
	/*! 
	 * \brief A virtual function.
	 * \param[in] first_elem - Identification of the first element.
	 * \param[in] second_elem - Identification of the second element.
	 * \param[in] face_first_elem - Index of the common face for the first element.
	 * \param[in] face_second_elem - Index of the common face for the second element.
	 */
	virtual bool FindFace(unsigned long first_elem, unsigned long second_elem, unsigned short &face_first_elem, 
						  unsigned short &face_second_elem);
	
	/*! 
	 * \brief A virtual member.
	 * \param[in] config - Definition of the particular problem.		 
	 */
	virtual void SetWall_Distance(CConfig *config);
	
	/*! 
	 * \brief A virtual member.
	 * \param[in] config - Definition of the particular problem.		 
	 */
	virtual void SetPositive_ZArea(CConfig *config);
	
	/*! 
	 * \brief A virtual member.
	 */
	virtual void SetEsuP(void);
	
	/*! 
	 * \brief A virtual member.
	 */	
	virtual void SetPsuP(void);
	
	/*! 
	 * \brief A virtual member.
	 */		
	virtual void SetEsuE(void);
	
	/*! 
	 * \brief A virtual member.
	 */
	void SetEdges(void);
	
	/*! 
	 * \brief A virtual member.
	 */
	virtual void SetBoundVolume(void);
	
	/*! 
	 * \brief A virtual member.
	 * \param[in] config - Definition of the particular problem.
	 */
	virtual void SetVertex(CConfig *config);
	
	/*! 
	 * \brief A virtual member.
	 */
	virtual void SetVertex(void);
	
	/*! 
	 * \brief A virtual member.
	 */		
	virtual void SetCG(void);
	
	/*! 
	 * \brief A virtual member.
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] action - Allocate or not the new elements.		 
	 */
	virtual void SetControlVolume(CConfig *config, unsigned short action);
  
	/*! 
	 * \brief A virtual member.
	 * \param[in] config - Definition of the particular problem.
	 */
	virtual void MachNearField(CConfig *config);
	
	/*! 
	 * \brief A virtual member.
	 * \param[in] config - Definition of the particular problem.
	 */
	virtual void MachInterface(CConfig *config);
	
	/*! 
	 * \brief A virtual member.
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] action - Allocate or not the new elements.		 
	 */
	virtual void SetBoundControlVolume(CConfig *config, unsigned short action);
	
	/*! 
	 * \brief A virtual member.
	 * \param[in] config_filename - Name of the file where the paraview information is going to be stored.
	 */
	virtual void SetParaView(char config_filename[200]);
	
	/*! 
	 * \brief A virtual member.
	 * \param[in] config_filename - Name of the file where the paraview information is going to be stored.
	 */
	virtual void SetTecPlot(char config_filename[200]);
	
	/*! 
	 * \brief A virtual member.
	 * \param[in] config - Definition of the particular problem.		 
	 * \param[in] mesh_filename - Name of the file where the paraview information is going to be stored.
	 */
	virtual void SetBoundParaView(CConfig *config, char mesh_filename[200]);
	
	/*! 
	 * \brief A virtual member.
	 * \param[in] config - Definition of the particular problem.		 
	 * \param[in] mesh_filename - Name of the file where the paraview information is going to be stored.
	 */
	virtual void SetBoundTecplot(CConfig *config, char mesh_filename[200]);
	
	/*! 
	 * \brief A virtual member.
	 * \param[in] config - Definition of the particular problem.		 
	 */
	virtual void Check_Orientation(CConfig *config);
	
	/*! 
	 * \brief A virtual member.
	 * \param[in] config - Definition of the particular problem.		 
	 * \param[in] val_ndomain - Number of domains for parallelization purposes.		 
	 */
	virtual void SetColorGrid(CConfig *config, unsigned short val_ndomain);
	
	/*! 
	 * \brief A virtual member.
	 * \param[in] config - Definition of the particular problem.		 
	 */
	virtual void SetPeriodicBoundary(CConfig *config);
	
	/*! 
	 * \brief A virtual member.
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] val_ndomain - Number of domains for parallelization purposes.		 
	 */
	virtual void SetSendReceive(CConfig *config, unsigned short val_ndomain);
	
	/*! 
	 * \brief A virtual member.
	 * \param[in] geometry - Geometrical definition of the problem.
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] val_domain - Number of domains for parallelization purposes.		 
	 */
	virtual void SetSendReceive(CGeometry *geometry, CConfig *config, unsigned short val_domain);

	/*! 
	 * \brief A virtual member.
	 * \param[in] geometry - Geometrical definition of the problem.
	 */	
	virtual void SetCoord(CGeometry *geometry);
	
	/*! 
	 * \brief A virtual member.
	 * \param[in] val_nSmooth - Number of smoothing iterations.
	 * \param[in] val_smooth_coeff - Relaxation factor.		 
	 */	
	virtual void SetCoord_Smoothing(unsigned short val_nSmooth, double val_smooth_coeff, CConfig *config);
	
	/*! 
	 * \brief A virtual member.
	 * \param[in] geometry - Geometrical definition of the problem.
	 */	
	virtual void SetPsuP(CGeometry *geometry);
	
	/*! 
	 * \brief A virtual member.
	 * \param[in] geometry - Geometrical definition of the problem.
	 * \param[in] config - Definition of the particular problem.
	 */	
	virtual void SetVertex(CGeometry *geometry, CConfig *config);
	
	/*! 
	 * \brief A virtual member.
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] geometry - Geometrical definition of the problem.
	 * \param[in] action - Allocate or not the new elements.		 
	 */	
	virtual void SetControlVolume(CConfig *config, CGeometry *geometry, unsigned short action);
	
	/*! 
	 * \brief A virtual member.
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] geometry - Geometrical definition of the problem.
	 * \param[in] action - Allocate or not the new elements.		 
	 */	
	virtual void SetBoundControlVolume(CConfig *config, CGeometry *geometry, unsigned short action);
	
	/*! 
	 * \brief A virtual member.
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] val_mesh_out_filename - Name of the output file.
	 */	
	virtual void SetMeshFile(CConfig *config, string val_mesh_out_filename);
	
	/*! 
	 * \brief A virtual member.
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] val_mesh_out_filename - Name of the output file.
	 */	
	virtual void SetMeshFile_IntSurface(CConfig *config, string val_mesh_out_filename);
	
	/*! 
	 * \brief A virtual member.
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] mesh_vtk - Name of the vtk file.
	 * \param[in] mesh_su2 - Name of the su2 file.
	 * \param[in] nslices - Number of slices of the 2D configuration.
	 */	
	virtual void Set3D_to_2D(CConfig *config, char mesh_vtk[200], char mesh_su2[200], unsigned short nslices);
	
	/*! 
	 * \brief A virtual member.
	 * \param[in] val_filename - Name of the file with the sensitivity information, be careful 
	 *            because as input file we don't use a .su2, in this case we use a .csv file.
	 */
	virtual void SetBoundSensitivity(string val_filename);
	
	/*! 
	 * \brief A virtual member.
	 * \param[in] geometry - Geometrical definition of the problem.
	 * \param[in] config - Definition of the particular problem.
	 */
	virtual void SetPeriodicBoundary(CGeometry *geometry, CConfig *config);
  
  /*! 
	 * \brief A virtual member.
	 * \param[in] config - Definition of the particular problem.
	 */
  virtual void SetRotationalVelocity(CConfig *config);
  
};

/*! 
 * \class CPhysicalGeometry
 * \brief Class for reading a defining the primal grid which is read from the 
 *        grid file in .su2 format.
 * \author F. Palacios.
 * \version 1.0.
 */
class CPhysicalGeometry : public CGeometry {
public:
	
	/*! 
	 * \brief Constructor of the class.
	 */
	CPhysicalGeometry(void);
	
	/*! 
	 * \overload
	 * \brief Reads the geometry of the grid and adjust the boundary 
	 *        conditions with the configuration file. 
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] val_mesh_filename - Name of the file with the grid information.
	 * \param[in] val_format - Format of the file with the grid information.
	 */
	CPhysicalGeometry(CConfig *config, string val_mesh_filename, unsigned short val_format);
	
	/*! 
	 * \brief Destructor of the class.
	 */
	~CPhysicalGeometry(void);
	
	/*! 
	 * \brief Set the right grid from Lockheed project.
	 */
	void SetLockheedGrid(CConfig *config);
	
	/*! 
	 * \brief Find repeated nodes between two elements to identify the common face.
	 * \param[in] first_elem - Identification of the first element.
	 * \param[in] second_elem - Identification of the second element.
	 * \param[in] face_first_elem - Index of the common face for the first element.
	 * \param[in] face_second_elem - Index of the common face for the second element.		 
	 * \return It provides 0 or 1 depending if there is a common face or not.
	 */
	bool FindFace(unsigned long first_elem, unsigned long second_elem, unsigned short &face_first_elem, 
				  unsigned short &face_second_elem);
	
	/*! 
	 * \brief Set elements which surround a point.
	 * \param[in] config - Definition of the particular problem.
	 */
	void SetWall_Distance(CConfig *config);
	
	/*! 
	 * \brief Compute positive z area for adimensionalization.
	 * \param[in] config - Definition of the particular problem.
	 */
	void SetPositive_ZArea(CConfig *config);
	
	/*! 
	 * \brief Set elements which surround a point.
	 */
	void SetEsuP(void);
	
	/*! 
	 * \brief Set points which surround a point.
	 */
	void SetPsuP(void);
	
	/*!
	 * \brief Function declaration to avoid partially overridden classes.
	 * \param[in] geometry - Geometrical definition of the problem.
	 */
	void SetPsuP(CGeometry *geometry);
	
	/*! 
	 * \brief Set elements which surround an element.
	 */
	void SetEsuE(void);
	
	/*! 
	 * \brief Set the volume element associated to each boundary element.
	 */
	void SetBoundVolume(void);
	
	/*! 
	 * \brief Set boundary vertex.
	 * \param[in] config - Definition of the particular problem.
	 */
	void SetVertex(CConfig *config);
	
	/*! 
	 * \brief Set the center of gravity of the face, elements and edges.
	 */
	void SetCG(void);
	
	/*! 
	 * \brief Set the edge structure of the control volume.
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] action - Allocate or not the new elements.
	 */
	void SetControlVolume(CConfig *config, unsigned short action);
  
	/*! 
	 * \brief Mach the near field boundary condition.
	 * \param[in] config - Definition of the particular problem.
	 */
	void MachNearField(CConfig *config);
	
	/*! 
	 * \brief Mach the interface boundary condition.
	 * \param[in] config - Definition of the particular problem.
	 */
	void MachInterface(CConfig *config);
	
	/*! 
	 * \brief Set boundary vertex structure of the control volume.
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] action - Allocate or not the new elements.
	 */
	void SetBoundControlVolume(CConfig *config, unsigned short action);
	
	/*! 
	 * \brief Set the Paraview file.
	 * \param[in] config_filename - Name of the file where the Paraview 
	 *            information is going to be stored.
	 */
	void SetParaView(char config_filename[200]);
	
	/*! 
	 * \brief Set the Tecplot file.
	 * \param[in] config_filename - Name of the file where the Tecplot 
	 *            information is going to be stored.
	 */
	void SetTecPlot(char config_filename[200]);
	
	/*! 
	 * \brief Set the output file for boundaries in Paraview
	 * \param[in] config - Definition of the particular problem.		 
	 * \param[in] mesh_filename - Name of the file where the Paraview 
	 *            information is going to be stored.
	 */
	void SetBoundParaView(CConfig *config, char mesh_filename[200]);
	
	/*! 
	 * \brief Set the output file for boundaries in Tecplot
	 * \param[in] config - Definition of the particular problem.		 
	 * \param[in] mesh_filename - Name of the file where the Tecplot 
	 *            information is going to be stored.
	 */
	void SetBoundTecplot(CConfig *config, char mesh_filename[200]);
	
	/*! 
	 * \brief Check the volume element orientation.
	 * \param[in] config - Definition of the particular problem.		 
	 */
	void Check_Orientation(CConfig *config);
	
	/*! 
	 * \brief Set the domains for grid grid partitioning.
	 * \param[in] config - Definition of the particular problem.		 
	 * \param[in] val_ndomain - Number of domains for parallelization purposes.		 
	 */
	void SetColorGrid(CConfig *config, unsigned short val_ndomain);
	
  /*! 
	 * \brief Set the rotational velocity at each grid point.
	 * \param[in] config - Definition of the particular problem.
	 */
  void SetRotationalVelocity(CConfig *config);
  
	/*! 
	 * \brief Set the periodic boundary conditions.
	 * \param[in] config - Definition of the particular problem.		 
	 */
	void SetPeriodicBoundary(CConfig *config);
	
	/*! 
	 * \brief Set the send receive boundaries.
	 * \param[in] config - Definition of the particular problem.		 
	 * \param[in] val_ndomain - Number of domains for parallelization purposes.		 
	 */
	void SetSendReceive(CConfig *config, unsigned short val_ndomain);
	
	/*! 
	 * \brief Do an implicit smoothing of the grid coordinates.
	 * \param[in] val_nSmooth - Number of smoothing iterations.
	 * \param[in] val_smooth_coeff - Relaxation factor.
	 * \param[in] config - Definition of the particular problem.		 
	 */	
	void SetCoord_Smoothing(unsigned short val_nSmooth, double val_smooth_coeff, CConfig *config);
	
	/*! 
	 * \brief Write the .su2 file.
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] val_mesh_out_filename - Name of the output file.
	 */	
	void SetMeshFile(CConfig *config, string val_mesh_out_filename);
	
	/*! 
	 * \brief Write the .su2 file, with an internal surface
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] val_mesh_out_filename - Name of the output file.
	 */	
	void SetMeshFile_IntSurface(CConfig *config, string val_mesh_out_filename);
	
	/*! 
	 * \brief Create a 2D mesh using a 3D mesh with symmetries.
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] mesh_vtk - Name of the vtk file.
	 * \param[in] mesh_su2 - Name of the su2 file.
	 * \param[in] nslices - Number of slices of the 2D configuration.
	 */	
	void Set3D_to_2D(CConfig *config, char mesh_vtk[200], char mesh_su2[200], unsigned short nslices);
	
	/*! 
	 * \brief Compute some parameters about the grid quality.
	 * \param[out] statistics - Information about the grid quality, statistics[0] = (r/R)_min, statistics[1] = (r/R)_ave.		 
	 */	
	void GetQualityStatistics(double *statistics);
	
};

/*! 
 * \class CMultiGridGeometry
 * \brief Class for defining the multigrid geometry, the main delicated part is the 
 *        agglomeration stage, which is done in the declaration.
 * \author F. Palacios.
 * \version 1.0.
 */
class CMultiGridGeometry : public CGeometry {
public:
	
	/*! 
	 * \brief Constructor of the class.
	 * \param[in] geometry - Geometrical definition of the problem.
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] iMesh - Level of the multigrid.
	 */	
	CMultiGridGeometry(CGeometry *geometry, CConfig *config, unsigned short iMesh);
	
	/*! 
	 * \brief Destructor of the class.
	 */
	~CMultiGridGeometry(void);
	
	/*! 
	 * \brief Determine if a CVPoint van be agglomerated, if it have the same marker point as the seed.
	 * \param[in] CVPoint - Control volume to be agglomerated.
	 * \param[in] marker_seed - Marker of the seed.
	 * \param[in] fine_grid - Geometrical definition of the problem.
	 * \param[in] config - Definition of the particular problem.
	 * \return True or false depending if the control volume can be agglomerated.
	 */	
	bool SetBoundAgglomeration(unsigned long CVPoint, short marker_seed, CGeometry *fine_grid, CConfig *config);
	
	/*! 
	 * \brief Determine if a CVPoint van be agglomerated, if it have the same marker point as the seed.
	 * \param[in] Suitable_Indirect_Neighbors - List of Indirect Neighbours that can be agglomerated.
	 * \param[in] iPoint - Seed point.
	 * \param[in] Index_CoarseCV - Index of agglomerated point.
	 * \param[in] fine_grid - Geometrical definition of the problem.
	 */		
	void SetSuitableNeighbors(vector<unsigned long> *Suitable_Indirect_Neighbors, unsigned long iPoint, 
							  unsigned long Index_CoarseCV, CGeometry *fine_grid);
		
	/*! 
	 * \brief Set boundary vertex.
	 * \param[in] geometry - Geometrical definition of the problem.
	 * \param[in] config - Definition of the particular problem.
	 */
	void SetVertex(CGeometry *geometry, CConfig *config);
	
	/*! 
	 * \brief Set points which surround a point.
	 * \param[in] geometry - Geometrical definition of the problem.
	 */	
	void SetPsuP(CGeometry *geometry);
	
	/*! 
	 * \brief Function declaration to avoid partially overridden classes.
	 */	
	void SetPsuP(void);
	
	/*! 
	 * \brief Set the edge structure of the agglomerated control volume.
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] geometry - Geometrical definition of the problem.
	 * \param[in] action - Allocate or not the new elements.
	 */	
	void SetControlVolume(CConfig *config, CGeometry *geometry, unsigned short action);
  
	/*! 
	 * \brief Mach the near field boundary condition.
	 * \param[in] config - Definition of the particular problem.
	 */
	void MachNearField(CConfig *config);
	
	/*! 
	 * \brief Mach the interface boundary condition.
	 * \param[in] config - Definition of the particular problem.
	 */
	void MachInterface(CConfig *config);
	
	/*! 
	 * \brief Set boundary vertex structure of the agglomerated control volume.
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] geometry - Geometrical definition of the problem.
	 * \param[in] action - Allocate or not the new elements.
	 */	
	void SetBoundControlVolume(CConfig *config, CGeometry *geometry, unsigned short action);
	
	/*! 
	 * \brief Set a representative coordinates of the agglomerated control volume.
	 * \param[in] geometry - Geometrical definition of the problem.
	 */	
	void SetCoord(CGeometry *geometry);
  
  /*! 
	 * \brief Set the rotational velocity at each grid point on a coarse mesh.
	 * \param[in] config - Definition of the particular problem.
	 */
  void SetRotationalVelocity(CConfig *config);
  
};

/*! 
 * \class CBoundaryGeometry
 * \brief Class for only defining the boundary of the geometry, this class is only 
 *        used in case we are not interested in the volumetric grid.
 * \author F. Palacios.
 * \version 1.0.
 */
class CBoundaryGeometry : public CGeometry {
public:
	
	/*! 
	 * \brief Constructor of the class.
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] val_mesh_filename - Name of the file with the grid information, be careful 
	 *            because as input file we don't use a .su2, in this case we use a .csv file.
	 * \param[in] val_format - Format of the file with the grid information.
	 */
	CBoundaryGeometry(CConfig *config, string val_mesh_filename, unsigned short val_format);
	
	/*! 
	 * \brief Destructor of the class.
	 */
	~CBoundaryGeometry(void);
	
	/*! 
	 * \brief Set boundary vertex.
	 */
	void SetVertex(void);

	/*! 
	 * \brief Compute the boundary geometrical structure.
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] action - Allocate or not the new elements.
	 */
	void SetBoundControlVolume(CConfig *config, unsigned short action);
	
	/*! 
	 * \brief Read the sensitivity froman input file.
	 * \param[in] val_filename - Name of the file with the sensitivity information, be careful 
	 *            because as input file we don't use a .su2, in this case we use a .csv file.
	 */
	void SetBoundSensitivity(string val_filename);
	
	/*! 
	 * \brief Set the output file for boundaries in Paraview
	 * \param[in] config - Definition of the particular problem.		 
	 * \param[in] mesh_filename - Name of the file where the Paraview 
	 *            information is going to be stored.
	 */
	void SetBoundParaView(CConfig *config, char mesh_filename[200]);
};

/*! 
 * \class CDomainGeometry
 * \brief Class for defining an especial kind of grid used in the partioning stage.
 * \author F. Palacios.
 * \version 1.0.
 */
class CDomainGeometry : public CGeometry {
	long *global_local_index;				/*!< \brief Global-local indexation for the points. */
	unsigned short *global_local_marker;	/*!< \brief Global-local marker. */
	unsigned long *local_global_index;				/*!< \brief Local-global indexation for the points. */

public:
	
	/*! 
	 * \brief Constructor of the class.
	 * \param[in] geometry - Geometrical definition of the problem.
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] val_domain - Number of domains for parallelization purposes.	 
	 */
	CDomainGeometry(CGeometry *geometry, CConfig *config, unsigned short val_domain);
	
	/*! 
	 * \brief Destructor of the class.
	 */
	~CDomainGeometry(void);
	
	/*! 
	 * \brief Set the send receive boundaries of the grid.
	 * \param[in] geometry - Geometrical definition of the problem.
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] val_domain - Number of domains for parallelization purposes.	 
	 */
	void SetSendReceive(CGeometry *geometry, CConfig *config, unsigned short val_domain);
	
	/*! 
	 * \brief Set the Paraview file.
	 * \param[in] config_filename - Name of the file where the Paraview 
	 *            information is going to be stored.
	 */
	void SetParaView(char config_filename[200]);
	
	/*! 
	 * \brief Set the Tecplot file.
	 * \param[in] config_filename - Name of the file where the Tecplot
	 *            information is going to be stored.
	 */
	void SetTecPlot(char config_filename[200]);
	
	/*! 
	 * \brief Set the output file for boundaries in Paraview
	 * \param[in] config - Definition of the particular problem.		 
	 * \param[in] mesh_filename - Name of the file where the Paraview
	 *            information is going to be stored.
	 */
	void SetBoundParaView(CConfig *config, char mesh_filename[200]);
	
	/*! 
	 * \brief Write the .su2 file.
	 * \param[in] config - Definition of the particular problem.		 
	 * \param[in] val_mesh_out_filename - Name of the output file.
	 */
	void SetMeshFile(CConfig *config, string val_mesh_out_filename);
};

/*! 
 * \class CPeriodicGeometry
 * \brief Class for defining a periodic boundary condition.
 * \author T. Economon, F. Palacios.
 * \version 1.0.
 */
class CPeriodicGeometry : public CGeometry {
  CPrimalGrid*** newBoundPer;            /*!< \brief Boundary vector for new periodic elements (primal grid information). */
  unsigned long *nNewElem_BoundPer;			/*!< \brief Number of new periodic elements of the boundary. */
  
public:
	
	/*! 
	 * \brief Constructor of the class.
	 * \param[in] geometry - Geometrical definition of the problem.
	 * \param[in] config - Definition of the particular problem.
	 */
	CPeriodicGeometry(CGeometry *geometry, CConfig *config);
	
	/*! 
	 * \brief Destructor of the class.
	 */
	~CPeriodicGeometry(void);
	
	/*! 
	 * \brief Set the periodic boundaries of the grid.
	 * \param[in] geometry - Geometrical definition of the problem.
	 * \param[in] config - Definition of the particular problem.
	 */
	void SetPeriodicBoundary(CGeometry *geometry, CConfig *config);
	
	/*! 
	 * \brief Set the Paraview file.
	 * \param[in] config_filename - Name of the file where the Paraview 
	 *            information is going to be stored.
	 */
	void SetParaView(char config_filename[200]);
	
	/*! 
	 * \brief Write the .su2 file.
	 * \param[in] config - Definition of the particular problem.		 
	 * \param[in] val_mesh_out_filename - Name of the output file.
	 */
	void SetMeshFile(CConfig *config, string val_mesh_out_filename);
};


#include "geometry_structure.inl"
