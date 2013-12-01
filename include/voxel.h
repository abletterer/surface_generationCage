#ifndef VOXEL_H
#define VOXEL_H

#include "Geometry/vector_gen.h"

namespace CGoGN
{
namespace SCHNApps
{

/*
* Classe définissant l'attribut de sommet Voxel, définissant l'affectation à un voxel
*/
class Voxel {
   public:
        Voxel(Geom::Vec3i voxel=Geom::Vec3i(-1,-1,-1))
        :   m_voxel(voxel)
        {}

        Geom::Vec3i getIndexes()
        {
            return m_voxel;
        }

        void setIndexes(Geom::Vec3i v)
        {
            m_voxel = Geom::Vec3i(v);
        }

        bool isInitialise()
        {
            return m_voxel.data()[0]!=-1;
        }

        bool operator==(const Voxel& v)
        {
            return  m_voxel.data()[0]==v.m_voxel.data()[0]
                    && m_voxel.data()[1]==v.m_voxel.data()[1]
                    && m_voxel.data()[2]==v.m_voxel.data()[2];
        }

        static std::string CGoGNnameOfType()
        {
            return "Voxel" ;
        }

    private:
        /*Liens dans l'arborescence*/
        Geom::Vec3i m_voxel;
};

} //namespace SCHNApps
} //namespace CGoGN

#endif // VOXEL_H
