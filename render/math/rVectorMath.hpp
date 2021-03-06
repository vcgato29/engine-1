/*!
 * \file rVectorMath.hpp
 * \brief \b Classes: \a rVectorMath
 */
/*
 * Copyright (C) 2015 EEnginE project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "defines.hpp"

#include "rMatrix.hpp"
#include <math.h>


namespace e_engine {

template <class T, int N>
using rVecN = rMatrix<T, N, 1>;
template <class T>
using rVec2 = rMatrix<T, 2, 1>;
template <class T>
using rVec3 = rMatrix<T, 3, 1>;
template <class T>
using rVec4 = rMatrix<T, 4, 1>;

/*!
 * | Element | Value |
 * | :-----: | :---: |
 * |    0    |   x   |
 * |    1    |   y   |
 * |    2    |   z   |
 * |    3    |   w   |
 */

typedef rMatrix<float, 2, 1> rVec2f;
typedef rMatrix<float, 3, 1> rVec3f;
typedef rMatrix<float, 4, 1> rVec4f;

typedef rMatrix<double, 2, 1> rVec2d;
typedef rMatrix<double, 3, 1> rVec3d;
typedef rMatrix<double, 4, 1> rVec4d;

template <int N>
using rVecNf = rMatrix<float, N, 1>;

template <int N>
using rVecNd = rMatrix<double, N, 1>;

class rVectorMath {
 public:
   template <class T, int N>
   static T dotProduct( const rVecN<T, N> &_vec1, const rVecN<T, N> &_vec2 );
   template <class T>
   static T dotProduct( const rVec2<T> &_vec1, const rVec2<T> &_vec2 );
   template <class T>
   static T dotProduct( const rVec3<T> &_vec1, const rVec3<T> &_vec2 );
   template <class T>
   static T dotProduct( const rVec4<T> &_vec1, const rVec4<T> &_vec2 );

   template <class T>
   static void quaternionMultiplication( const rVec4<T> &_q1, const rVec4<T> &_q2, rVec4<T> &_out );

   template <class T>
   static rVec3<T> crossProduct( const rVec3<T> &_vec1, const rVec3<T> &_vec2 );
};


//  ______      _    ______              _            _
//  |  _  \    | |   | ___ \            | |          | |
//  | | | |___ | |_  | |_/ / __ ___   __| |_   _  ___| |_
//  | | | / _ \| __| |  __/ '__/ _ \ / _` | | | |/ __| __|
//  | |/ / (_) | |_  | |  | | | (_) | (_| | |_| | (__| |_
//  |___/ \___/ \__| \_|  |_|  \___/ \__,_|\__,_|\___|\__|
//

template <class T, int N>
T rVectorMath::dotProduct( const rVecN<T, N> &_vec1, const rVecN<T, N> &_vec2 ) {
   T lProduct = 0;

   for ( int i = 0; i < N; ++i )
      lProduct += _vec1[i] * _vec2[i];

   return lProduct;
}

template <class T>
T rVectorMath::dotProduct( const rVec2<T> &_vec1, const rVec2<T> &_vec2 ) {
   return _vec1.x * _vec2.x + _vec1.y * _vec2.y;
}

template <class T>
T rVectorMath::dotProduct( const rVec3<T> &_vec1, const rVec3<T> &_vec2 ) {
   return _vec1.x * _vec2.x + _vec1.y * _vec2.y + _vec1.z * _vec2.z;
}

template <class T>
T rVectorMath::dotProduct( const rVec4<T> &_vec1, const rVec4<T> &_vec2 ) {
   return _vec1.x * _vec2.x + _vec1.y * _vec2.y + _vec1.z * _vec2.z + _vec1.w * _vec2.w;
}


//   _____                    ______              _            _
//  /  __ \                   | ___ \            | |          | |
//  | /  \/_ __ ___  ___ ___  | |_/ / __ ___   __| |_   _  ___| |_
//  | |   | '__/ _ \/ __/ __| |  __/ '__/ _ \ / _` | | | |/ __| __|
//  | \__/\ | | (_) \__ \__ \ | |  | | | (_) | (_| | |_| | (__| |_
//   \____/_|  \___/|___/___/ \_|  |_|  \___/ \__,_|\__,_|\___|\__|
//


template <class T>
rVec3<T> rVectorMath::crossProduct( const rVec3<T> &_vec1, const rVec3<T> &_vec2 ) {
   return rVec3<T>( ( _vec1.y * _vec2.z ) - ( _vec1.z * _vec2.y ),
                    ( _vec1.z * _vec2.x ) - ( _vec1.x * _vec2.z ),
                    ( _vec1.x * _vec2.y ) - ( _vec1.y * _vec2.x ) );
}

/*
(Q1 * Q2).x = (w1x2 + x1w2 + y1z2 - z1y2)
(Q1 * Q2).y = (w1y2 - x1z2 + y1w2 + z1x2)
(Q1 * Q2).z = (w1z2 + x1y2 - y1x2 + z1w2)
(Q1 * Q2).w = (w1w2 - x1x2 - y1y2 - z1z2)
*/

template <class T>
void rVectorMath::quaternionMultiplication( const rVec4<T> &_q1,
                                            const rVec4<T> &_q2,
                                            rVec4<T> &      _out ) {
   _out.x = ( _q1.w * _q2.x ) + ( _q1.x * _q2.w ) + ( _q1.y * _q2.z ) - ( _q1.z * _q2.y );
   _out.y = ( _q1.w * _q2.y ) - ( _q1.x * _q2.z ) + ( _q1.y * _q2.w ) + ( _q1.z * _q2.x );
   _out.z = ( _q1.w * _q2.z ) + ( _q1.x * _q2.y ) - ( _q1.y * _q2.x ) + ( _q1.z * _q2.w );
   _out.w = ( _q1.w * _q2.w ) - ( _q1.x * _q2.x ) - ( _q1.y * _q2.y ) - ( _q1.z * _q2.z );
}
}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
