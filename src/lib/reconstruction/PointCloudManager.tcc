/* Copyright (C) 2011 Uni Osnabrück
 * This file is part of the LAS VEGAS Reconstruction Toolkit,
 *
 * LAS VEGAS is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * LAS VEGAS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA
 */


 /*
 * PointCloudManager.tcc
 *
 *  Created on: 02.03.2011
 *      Author: Thomas Wiemann
 */

#include <cassert>
#include <string>
#include <cmath>
#include <cstring>

#include "../io/Timestamp.hpp"
#include "../io/ModelFactory.hpp"

#include <boost/filesystem.hpp>

namespace lssr
{


template<typename VertexT, typename NormalT>
BoundingBox<VertexT>& PointCloudManager<VertexT, NormalT>::getBoundingBox()
{
    return m_boundingBox;
}


template<typename VertexT, typename NormalT>
VertexT PointCloudManager<VertexT, NormalT>::getPoint(size_t index)
{
    assert(index < m_numPoints);
    return VertexT(
            m_points[index][0], m_points[index][1], m_points[index][2], 
            m_colors[index][0], m_colors[index][1], m_colors[index][2] );
}


template<typename VertexT, typename NormalT>
size_t PointCloudManager<VertexT, NormalT>::getNumPoints()
{
    return m_numPoints;
}


template<typename VertexT, typename NormalT>
const VertexT PointCloudManager<VertexT, NormalT>::operator[]( const size_t& index ) const
{
    return VertexT( m_points[index][0], m_points[index][1], m_points[index][2] );
}


template<typename VertexT, typename NormalT>
void PointCloudManager<VertexT, NormalT>::colorizePointCloud( 
        PointCloudManager<VertexT, NormalT>* pcm, const float &maxDist,
        const uchar* blankColor )
{

#pragma omp parallel for
    /* Run through laserscan cloud and find neighbours. */
    for ( int i = 0; i < m_numPoints; i++ ) {

        std::vector<VertexT> nearestPoint(1);

        /* nearest neighbor search */
        VertexT p( this->getPoint( i ) );
        pcm->getkClosestVertices( p, 1, nearestPoint );
        /* Check if vector contains point. */
        if ( nearestPoint.size() ) {
            VertexT q( nearestPoint[0] );
            float dist = sqrt( 
                      ( p.x - q.x ) * ( p.x - q.x ) 
                    + ( p.y - q.y ) * ( p.y - q.y ) 
                    + ( p.z - q.z ) * ( p.z - q.z ) );
            if ( dist && dist > maxDist && blankColor ) {
                /* Set default color. */
                m_colors[i][0] = blankColor[0];
                m_colors[i][1] = blankColor[1];
                m_colors[i][2] = blankColor[2];
            } else {
                /* Get color from other pointcloud. */
                m_colors[i][0] = q.r;
                m_colors[i][1] = q.g;
                m_colors[i][2] = q.b;
            }
            /* TODO: Store the distance as confidence information. */

        }
    }

}


}

