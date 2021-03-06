/*!
 *****************************************************************
 * \file
 *
 * \note
 *   Copyright (c) 2012 \n
 *   Fraunhofer Institute for Manufacturing Engineering
 *   and Automation (IPA) \n\n
 *
 *****************************************************************
 *
 * \note
 *  Project name: care-o-bot
 * \note
 *  ROS stack name: cob_environment_perception
 * \note
 *  ROS package name: cob_3d_mapping_geometry_map
 *
 * \author
 *  Author: Georg Arbeiter, email:georg.arbeiter@ipa.fhg.de
 * \author
 *  Supervised by: Georg Arbeiter, email:georg.arbeiter@ipa.fhg.de
 *
 * \date Date of creation: 08/2011
 *
 * \brief
 * Description: Feature Map for storing and handling geometric features
 *
 * ToDo:
 *
 *
 *
 *****************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     - Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer. \n
 *     - Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution. \n
 *     - Neither the name of the Fraunhofer Institute for Manufacturing
 *       Engineering and Automation (IPA) nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission. \n
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License LGPL as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License LGPL for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License LGPL along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 *
 ****************************************************************/

//##################
//#### includes ####
// standard includes
#include <sstream>
#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <queue>
#include <functional>

// external includes
#include <boost/timer.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include <Eigen/Geometry>
//#include <pcl/win32_macros.h> // not available anymore in pcl 1.7
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <pcl/common/centroid.h>

//#include <cob_3d_mapping_slam/dof/tflink.h>
#include "cob_3d_mapping_geometry_map/geometry_map.h"

using namespace cob_3d_mapping;

void
GeometryMap::addMapEntry (const GeometryMapEntry::Ptr& p)
{
	cob_3d_mapping::MergeConfig limits;
	limits.d_thresh = d_;
	limits.angle_thresh = cos_angle_;
	limits.weighting_method = "AREA";	//"COMBINED", "COUNTER"
	p->setMergeSettings(limits);
	p->setHeader(new_id_++, frame_counter_);
	
	std::vector<size_t> intersections;
	for(size_t i=0; i<map_entries_.size(); i++) {
		if(p->isMergeCandidate(map_entries_[i]))
			intersections.push_back(i);
	}
	for(size_t i=intersections.size()-1; i!=(size_t)-1; i--) {
		if(p->merge(map_entries_[intersections[i]]))
			map_entries_.erase(map_entries_.begin()+intersections[i]);
		else
			map_entries_[intersections[i]]->setHeader(map_entries_[intersections[i]]->getId(), frame_counter_);
	}
	map_entries_.push_back(p);
  
	if (save_to_file_)
		saveMap (file_path_);
}

void
GeometryMap::addMapEntry (const Polygon::Ptr& p_ptr, const bool merge)
{
	if(merge)
		addMapEntry(GeometryMapEntry::Ptr(new GeometryMapEntry_Polygon(p_ptr)));
	else
		map_entries_.push_back(GeometryMapEntry::Ptr(new GeometryMapEntry_Polygon(p_ptr)));
}

void
GeometryMap::addMapEntry (const Cylinder::Ptr& c_ptr, const bool merge)
{
	if(merge)
		addMapEntry(GeometryMapEntry::Ptr(new GeometryMapEntry_Cylinder(c_ptr)));
	else
		map_entries_.push_back(GeometryMapEntry::Ptr(new GeometryMapEntry_Polygon(c_ptr)));
}

/*void
 GeometryMap::addMapEntry(ShapeCluster::Ptr& sc_ptr)
 {
 sc_ptr->computeAttributes();
 if (map_shape_cluster_.size())
 {
 std::vector<int> intersections;
 sc_ptr->getMergeCandidates(map_shape_cluster_, intersections);
 ROS_DEBUG_STREAM(intersections.size());
 if(intersections.size())
 {
 std::vector<ShapeCluster::Ptr> do_merge;
 for(int i=intersections.size()-1; i>=0; --i)
 {
 do_merge.push_back(map_shape_cluster_[intersections[i]]);
 map_shape_cluster_[intersections[i]] = map_shape_cluster_.back();
 map_shape_cluster_.pop_back();
 }
 sc_ptr->merge(do_merge);
 sc_ptr->id = new_id_++;
 map_shape_cluster_.push_back(sc_ptr);
 }
 else
 {
 sc_ptr->id = new_id_++;
 sc_ptr->frame_stamp = frame_counter_;
 map_shape_cluster_.push_back(sc_ptr);
 }
 }
 else
 {
 sc_ptr->id = new_id_++;
 sc_ptr->frame_stamp = frame_counter_;
 map_shape_cluster_.push_back(sc_ptr);
 }
 }*/

/*bool
 GeometryMap::computeTfError(const std::vector<Polygon::Ptr>& list_polygon, const Eigen::Affine3f& tf_old, Eigen::Affine3f& adjust_tf)
 {
 return false;
 if (map_polygon_.size() < 10)
 {
 adjust_tf = Eigen::Affine3f::Identity();
 last_tf_err_ = Eigen::Affine3f::Identity();
 return false;
 }
 cob_3d_mapping::merge_config  limits;
 limits.d_thresh=d_;
 limits.angle_thresh=cos_angle_;
 limits.weighting_method="COUNTER";
 // min heap to store polygons with max overlap (Landmark elements: overlap, num_vertices, idx_old, idx_new)
 typedef boost::tuple<float,unsigned int,unsigned int> Landmark;
 std::priority_queue<Landmark> landmarks_queue;
 const size_t q_size = 3;
 int sum_overlap = 0;
 for (size_t p=0; p<map_polygon_.size(); ++p) // old polys
 {
 Polygon::Ptr pp = map_polygon_[p];

 for (size_t q=0; q<list_polygon.size(); ++q) // new polys
 {
 Polygon::Ptr pq = list_polygon[q];
 pq->merge_settings_=limits;
 if ( !pp->hasSimilarParametersWith(pq) ) continue;
 //if ( pq->contours[pq->outerContourIndex()].size() < 20 ) continue;
 //int abs_overlap;
 //float rel_overlap;
 //if (!pp->getContourOverlap(pq, rel_overlap, abs_overlap)) continue;
 //if (abs_overlap < 10) continue;
 //if (rel_overlap < 0.3) continue;
 //sum_overlap += abs_overlap;
 float w = pp->computeSimilarity(pq);
 ROS_DEBUG_STREAM("Sim: " << w);
 if (w < 0.70) continue;
 landmarks_queue.push( Landmark(w, p, q) );
 }
 }
 if (landmarks_queue.size() < q_size) return false;

 Landmark lm;
 DOF6::TFLinkvf tfe;
 Eigen::Vector3f n,m;
 float d1, d2;
 int i = 0;
 while (landmarks_queue.size() != 0)
 {
 lm = landmarks_queue.top(); landmarks_queue.pop();
 n = map_polygon_[lm.get<1>()]->normal;
 m = list_polygon[lm.get<2>()]->normal;
 d1 = map_polygon_[lm.get<1>()]->d;
 d2 = list_polygon[lm.get<2>()]->d;
 float weight = 2.0f/(fabs(d1)+fabs(d2));//(float)lm.get<0>();
 tfe(DOF6::TFLinkvf::TFLinkObj( d2 * m , true, false, weight),
 DOF6::TFLinkvf::TFLinkObj( d1 * n , true, false, weight));

 ROS_DEBUG_STREAM("%Overlap: "<<lm.get<0>()<<" Weigth: "<<weight);
 ROS_DEBUG_STREAM("%Area(old/new): "<<map_polygon_[lm.get<1>()]->computeArea3d()<<", "
 <<list_polygon[lm.get<2>()]->computeArea3d());
 ROS_DEBUG_STREAM("vector_a"<<i<<" = ["<<n(0)<<","<<n(1)<<","<<n(2)<<"];");
 ROS_DEBUG_STREAM("vector_b"<<i<<" = ["<<m(0)<<","<<m(1)<<","<<m(2)<<"];");
 ROS_DEBUG_STREAM("origin_a"<<i<<" = "<<d1<<" * vector_a"<<i<<";");
 ROS_DEBUG_STREAM("origin_b"<<i<<" = "<<d2<<" * vector_b"<<i<<";");
 ++i;
 }

 tfe.finish();
 Eigen::Affine3f tf;
 tf.matrix().topLeftCorner<3,3>() = tfe.getRotation();
 tf.matrix().topRightCorner<3,1>() = tfe.getTranslation();
 tf.matrix().bottomLeftCorner<1,4>() << 0, 0, 0, 1;

 float roll, pitch, yaw;
 pcl::getEulerAngles(tf, roll, pitch, yaw);
 ROS_DEBUG_STREAM("Angles: r="<<roll*180.0f/M_PI<<" p="<<pitch*180.0f/M_PI<<" y="<<yaw*180.0f/M_PI);

 adjust_tf = tf;
 last_tf_err_ = adjust_tf;

 return true;
 }*/

void
GeometryMap::cleanUp ()
{
	for(size_t j=0; j<map_entries_.size(); j++) {
		std::vector<size_t> intersections;
		for(size_t i=j+1; i<map_entries_.size(); i++) {
			if(map_entries_[j]->isMergeCandidate(map_entries_[i]))
				intersections.push_back(i);
		}
		for(size_t i=intersections.size()-1; i!=(size_t)-1; i--) {
			if(map_entries_[j]->merge(map_entries_[intersections[i]]))
				map_entries_.erase(map_entries_.begin()+intersections[i]);
			else
				map_entries_[intersections[i]]->setHeader(map_entries_[intersections[i]]->getId(), frame_counter_);
		}
		
		if(intersections.size()>0) 
			map_entries_[j]->setHeader(map_entries_[j]->getId(), frame_counter_);	//update timestamp to last merge
	}
	
  int n_dropped = 0, m_dropped = 0, c_dropped = 0;
  for (int idx = map_entries_.size () - 1; idx >= 0; --idx)
  {
    if(map_entries_[idx]->needsCleaning(frame_counter_))
    {
      //ROS_INFO ("cleaning id %d", idx);
      map_entries_[idx] = map_entries_.back ();
      map_entries_.pop_back ();
      ++n_dropped;
    }
  }

  /*for(int idx = map_cylinder_.size() - 1 ; idx >= 0; --idx)
   {
   bool drop_cyl=false;
   ROS_DEBUG_STREAM("merged:"<<(int)map_cylinder_[idx]->merged_<<" frame ctr:"<<frame_counter_<<" frame st:"<<(int)map_cylinder_[idx]->frame_stamp_<<" size:"<<(int)map_cylinder_[idx]->contours_[0].size());

   if (map_cylinder_[idx]->merged_ <= 1 && (frame_counter_ - 2) > (int)map_cylinder_[idx]->frame_stamp_)
   {
   drop_cyl=true;
   }
   // TODO<<<<WATCH OUT<<<<< presentation configuration - hard coded limits >>>>>>>>>>>>>>>>>>
   if ((int)map_cylinder_[idx]->contours_[0].size()<30 && (int)map_cylinder_[idx]->merged_ <= 1)
   {
   drop_cyl=true;
   }
   if (map_cylinder_[idx]->r_ < 0.1 || map_cylinder_[idx]->r_>0.2)
   {
   drop_cyl=true;
   }
   if ( drop_cyl==true)
   {
   map_cylinder_[idx] = map_cylinder_.back();
   map_cylinder_.pop_back();
   ++c_dropped;
   }
   }
   for(int idx = map_shape_cluster_.size() - 1 ; idx >= 0; --idx)
   {
   ROS_DEBUG_STREAM( map_shape_cluster_[idx]->merged_ <<", " << (frame_counter_ - 3) <<" > "<<(int)map_shape_cluster_[idx]->frame_stamp_);
   if (map_shape_cluster_[idx]->merged_ <= 1 && (frame_counter_ - 3) > (int)map_shape_cluster_[idx]->frame_stamp_)
   {
   map_shape_cluster_[idx] = map_shape_cluster_.back();
   map_shape_cluster_.pop_back();
   ++m_dropped;
   }
   }*/
  //ROS_DEBUG_STREAM ("Dropped " << n_dropped << " Polys, " << c_dropped << " Cyls, " << m_dropped << " Clusters");
}

void
GeometryMap::saveMap (std::string path)
{

  //	only for polygons
  static int ctr = 0;
  std::stringstream ss;
  ss << path << "/" << ctr << "_";
  for (size_t i = 0; i < map_entries_.size (); i++)
  {
	std::stringstream ss2;
	ss2 << ss.str() << "map_entry_" << i;
    map_entries_[i]->save(ss2.str());
  }
  ctr++;
}

void
GeometryMap::clearMap ()
{
  map_entries_.clear();
}

void
GeometryMap::colorizeMap ()
{
	for(entry_iterator it = map_entries_.begin(); it!=map_entries_.end(); it++)
		(*it)->colorize();
}

GeometryMapEntry::Ptr *GeometryMap::getMapEntry(const int id) {
	for(entry_iterator it = map_entries_.begin(); it!=map_entries_.end(); it++)
		if((*it)->getId()==id)
			return &(*it);
	return NULL;
}

void GeometryMap::eraseMapEntry(const int id) {
	for(size_t i=map_entries_.size()-1; i!=(size_t)-1; i--)
		if(map_entries_[i]->getId()==id)
			map_entries_.erase(map_entries_.begin()+i);
}

void GeometryMap::checkVisibility(const Eigen::Affine3f &T, const Eigen::Vector3f &camera_params) {
	const Eigen::Vector3f Z = Eigen::Vector3f::UnitZ();
	
	for (int idx = map_entries_.size () - 1; idx >= 0; --idx)
	{
		if( map_entries_[idx]->needsCleaning(frame_counter_) ||
			(map_entries_[idx]->checkVisibility(T,camera_params,Z)&&map_entries_[idx]->needsCleaning(frame_counter_, false)) )
		{
		  //ROS_INFO ("cleaning id %d", idx);
		  map_entries_[idx] = map_entries_.back ();
		  map_entries_.pop_back ();
		}
	}
}

