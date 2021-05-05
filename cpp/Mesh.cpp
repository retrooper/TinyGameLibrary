#include "Mesh.h"
namespace tgl {
    bool MeshDescription::operator==(const MeshDescription& other) const {
        if (vertices.size() != other.vertices.size()) {
            return false;
        }
        if (indices.size() != other.indices.size()) {
            return false;
        }

        for (int i = 0; i < vertices.size(); i++) {
            Vertex localVertex = vertices[i];
            Vertex oppVertex = other.vertices[i];
            if (localVertex != oppVertex) {
                return false;
            }
        }

        for (int i = 0; i < indices.size(); i++) {
            int localIndex = indices[i];
            int oppIndex = other.indices[i];
            if (localIndex != oppIndex) {
                return false;
            }
        }
        return true;
    }

    bool MeshDescription::operator<(const MeshDescription &other) const {
        return true;
    }

}