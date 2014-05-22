#ifndef CORE_MESH_MESHDATA_H_
#define CORE_MESH_MESHDATA_H_

namespace ml {

//! raw mesh data could be also a point cloud
template <class FloatType>
class MeshData {
public:
	MeshData() {}
	MeshData(MeshData&& d) {
		m_Vertices = std::move(d.m_Vertices);
		m_Normals = std::move(d.m_Normals);
		m_TextureCoords = std::move(d.m_TextureCoords);
		m_Colors = std::move(d.m_Colors);
		m_FaceIndicesVertices = std::move(d.m_FaceIndicesVertices);
		m_FaceIndicesNormals = std::move(d.m_FaceIndicesNormals);
		m_FaceIndicesTextureCoords = std::move(d.m_FaceIndicesTextureCoords);
		m_FaceIndicesColors = std::move(d.m_FaceIndicesColors);
	}
	void operator=(MeshData&& d) {
		m_Vertices = std::move(d.m_Vertices);
		m_Normals = std::move(d.m_Normals);
		m_TextureCoords = std::move(d.m_TextureCoords);
		m_Colors = std::move(d.m_Colors);
		m_FaceIndicesVertices = std::move(d.m_FaceIndicesVertices);
		m_FaceIndicesNormals = std::move(d.m_FaceIndicesNormals);
		m_FaceIndicesTextureCoords = std::move(d.m_FaceIndicesTextureCoords);
		m_FaceIndicesColors = std::move(d.m_FaceIndicesColors);
	}
	void clear() {
		m_Vertices.clear();
		m_Normals.clear();
		m_Colors.clear();
		m_FaceIndicesVertices.clear();
		m_FaceIndicesNormals.clear();
		m_FaceIndicesTextureCoords.clear();
		m_FaceIndicesColors.clear();
	}

	bool isConsistent(bool detailedCheck = false) const {

		bool consistent = true;
		if (m_FaceIndicesNormals.size() > 0			&& m_FaceIndicesVertices.size() != m_FaceIndicesNormals.size())			consistent = false;
		if (m_FaceIndicesTextureCoords.size() > 0	&& m_FaceIndicesVertices.size() != m_FaceIndicesTextureCoords.size())	consistent = false;
		if (m_FaceIndicesColors.size() > 0			&& m_FaceIndicesColors.size() != m_FaceIndicesColors.size())			consistent = false;

		if (hasPerVertexNormals() && m_Vertices.size() != m_Normals.size())			consistent = false;
		if (hasPerVertexTexCoords() && m_Vertices.size() != m_TextureCoords.size()) consistent = false;
		if (hasPerVertexColors() && m_Vertices.size() != m_Colors.size())			consistent = false;

		if (detailedCheck) {
			//make sure no index is out of bounds
			for (auto& face : m_FaceIndicesVertices) {
				for (auto& idx : face) {
					if (idx >= m_Vertices.size())	consistent = false;
				}
			}
			for (auto& face : m_FaceIndicesColors) {
				for (auto& idx : face) {
					if (idx >= m_Colors.size())	consistent = false;
				}
			}
			for (auto& face : m_FaceIndicesNormals) {
				for (auto& idx : face) {
					if (idx >= m_FaceIndicesNormals.size())	consistent = false;
				}
			}
		}
		return consistent;
	}

	void applyTransform(const mat4f& t) {
		for (size_t i = 0; i < m_Vertices.size(); i++) {
			m_Vertices[i] = t*m_Vertices[i];
		}
		mat4f invTrans = t.getInverse().getTranspose();
		for (size_t i = 0; i < m_Normals.size(); i++) {
			m_Normals[i] = invTrans*m_Normals[i];
		}
	}

	BoundingBox3d<FloatType> getBoundingBox() const {
		BoundingBox3d<FloatType> bb;
		for (size_t i = 0; i < m_Vertices.size(); i++) {
			bb.include(m_Vertices[i]);
		}
		return bb;
	}

	const std::vector<std::vector<unsigned int>>& getFaceIndicesVertices() const {
		return m_FaceIndicesVertices;
	}
	const std::vector<std::vector<unsigned int>>& getFaceIndicesNormals() const {
		if (!hasNormals())	throw MLIB_EXCEPTION("mesh does not have normals");
		else if (m_FaceIndicesNormals.size() > 0)			return m_FaceIndicesNormals;
		else if (m_Vertices.size() == m_Normals.size())		return m_FaceIndicesVertices;
		else throw MLIB_EXCEPTION("vertex/normal mismatch");
	}
	const std::vector<std::vector<unsigned int>>& getFaceIndicesTexCoords() const {
		if (!hasTexCoords())	throw MLIB_EXCEPTION("mesh does not have texcoords");
		else if (m_FaceIndicesTextureCoords.size() > 0)			return m_FaceIndicesTextureCoords;
		else if (m_Vertices.size() == m_TextureCoords.size())	return m_FaceIndicesVertices;
		else throw MLIB_EXCEPTION("vertex/texcoord mismatch");
	}
	const std::vector<std::vector<unsigned int>>& getFaceIndicesColors() const {
		if (!hasColors())	throw MLIB_EXCEPTION("mesh does not have colors");
		else if (m_FaceIndicesColors.size() > 0)		return m_FaceIndicesColors;
		else if (m_Vertices.size() == m_Colors.size())	return m_FaceIndicesVertices;
		else throw MLIB_EXCEPTION("vertex/color mismatch");
	}

	bool hasNormals() const { return m_Normals.size() > 0; }
	bool hasColors() const { return m_Colors.size() > 0; }
	bool hasTexCoords() const { return m_TextureCoords.size() > 0; }

	bool hasPerVertexNormals()	const	{ return hasNormals() && m_FaceIndicesNormals.size() == 0; }
	bool hasPerVertexColors()	const	{ return hasColors() && m_FaceIndicesColors.size() == 0; }
	bool hasPerVertexTexCoords() const	{ return hasTexCoords() && m_FaceIndicesTextureCoords.size() == 0; }

	bool hasVertexIndices() const { return m_FaceIndicesVertices.size() > 0; }
	bool hasColorIndices() const { return m_FaceIndicesColors.size() > 0; }
	bool hasNormalIndices() const { return m_FaceIndicesNormals.size() > 0; }
	bool hasTexCoordsIndices() const { return m_FaceIndicesTextureCoords.size() > 0; }


	//! merges two meshes (assumes the same memory layout/type)
	void merge(const MeshData<FloatType>& other) {
		////TODO just delete if non existent in other mesh
		//assert(
		//	hasNormals() == other.hasNormals() &&
		//	hasColors() == other.hasColors() &&
		//	hasTexCoords() == other.hasTexCoords() &&
		//	hasPerVertexNormals() == other.hasPerVertexNormals() &&
		//	hasPerVertexTexCoords() == other.hasPerVertexTexCoords() &&
		//	hasPerVertexColors() == other.hasPerVertexColors() &&
		//	hasVertexIndices() == other.hasVertexIndices() &&
		//	hasColorIndices() == other.hasColorIndices() &&
		//	hasTexCoordsIndices() == other.hasTexCoordsIndices()
		//);

		if (hasVertexIndices() != other.hasVertexIndices()) throw MLIB_EXCEPTION("invalid mesh conversion");

		if (hasNormals() != other.hasNormals() || hasNormalIndices() != other.hasNormalIndices()) {
			m_Normals.clear();
			m_FaceIndicesNormals.clear();
		}
		if (hasColors() != other.hasColors() || hasColorIndices() != other.hasColorIndices()) {
			m_Colors.clear();
			m_FaceIndicesColors.clear();
		}
		if (hasTexCoords() != other.hasTexCoords() || hasTexCoordsIndices() != other.hasTexCoordsIndices()) {
			m_TextureCoords.clear();
			m_FaceIndicesTextureCoords.clear();
		}

		size_t vertsBefore = m_Vertices.size();
		size_t normsBefore = m_Normals.size();
		size_t colorBefore = m_Colors.size();
		size_t texCoordsBefore = m_TextureCoords.size();
		m_Vertices.insert(m_Vertices.end(), other.m_Vertices.begin(), other.m_Vertices.end());
		if (hasColors())	m_Colors.insert(m_Colors.end(), other.m_Colors.begin(), other.m_Colors.end());
		if (hasNormals())	m_Normals.insert(m_Normals.end(), other.m_Normals.begin(), other.m_Normals.end());
		if (hasTexCoords())	m_TextureCoords.insert(m_TextureCoords.end(), other.m_TextureCoords.begin(), other.m_TextureCoords.end());

		if (hasVertexIndices()) {
			size_t indicesBefore = m_FaceIndicesVertices.size();
			m_FaceIndicesVertices.insert(m_FaceIndicesVertices.end(), other.m_FaceIndicesVertices.begin(), other.m_FaceIndicesVertices.end());
			for (size_t i = indicesBefore; i < m_FaceIndicesVertices.size(); i++) {
				for (auto& idx : m_FaceIndicesVertices[i]) idx += (unsigned int)vertsBefore;
			}
		}
		if (hasNormalIndices()) {
			size_t indicesBefore = m_FaceIndicesNormals.size();
			m_FaceIndicesNormals.insert(m_FaceIndicesNormals.end(), other.m_FaceIndicesNormals.begin(), other.m_FaceIndicesNormals.end());
			for (size_t i = indicesBefore; i < m_FaceIndicesNormals.size(); i++) {
				for (auto& idx : m_FaceIndicesNormals[i]) idx +=  (unsigned int)normsBefore;
			}
		}
		if (hasColorIndices()) {
			size_t indicesBefore = m_FaceIndicesColors.size();
			m_FaceIndicesColors.insert(m_FaceIndicesColors.end(), other.m_FaceIndicesColors.begin(), other.m_FaceIndicesColors.end());
			for (size_t i = indicesBefore; i < m_FaceIndicesColors.size(); i++) {
				for (auto& idx : m_FaceIndicesColors[i]) idx +=  (unsigned int)colorBefore;
			}
		}
		if (hasTexCoordsIndices()) {
			size_t indicesBefore = m_FaceIndicesTextureCoords.size();
			m_FaceIndicesTextureCoords.insert(m_FaceIndicesTextureCoords.end(), other.m_FaceIndicesTextureCoords.begin(), other.m_FaceIndicesTextureCoords.end());
			for (size_t i = indicesBefore; i < m_FaceIndicesTextureCoords.size(); i++) {
				for (auto& idx : m_FaceIndicesTextureCoords[i]) idx +=  (unsigned int)texCoordsBefore;
			}
		}
	}
	unsigned int removeDuplicateVertices();
	unsigned int removeDuplicateFaces();
	unsigned int mergeCloseVertices(FloatType thresh, bool approx = false);
	unsigned int removeDegeneratedFaces();

	//! also removes isolated normals, colors, etc.
	unsigned int removeIsolatedVertices();

	//! removes all the vertices that are behind a plane (faces with one or more of those vertices are being deleted as well)
	unsigned int removeVerticesBehindPlane(const Plane<FloatType>& plane, FloatType thresh);

	std::vector<point3d<FloatType>>	m_Vertices;			//vertices are indexed (see below)
	std::vector<point3d<FloatType>>	m_Normals;			//normals are indexed (see below/or per vertex)
	std::vector<point2d<FloatType>>	m_TextureCoords;	//tex coords are indexed (see below/or per vertex)
	std::vector<point4d<FloatType>>	m_Colors;			//colors are not indexed (see below/or per vertex) 
	std::vector<std::vector<unsigned int>>	m_FaceIndicesVertices;		//indices in face array
	std::vector<std::vector<unsigned int>>	m_FaceIndicesNormals;		//indices in normal array (if size==0, indicesVertices is used)
	std::vector<std::vector<unsigned int>>	m_FaceIndicesTextureCoords;	//indices in texture array (if size==0, indicesVertices is used)
	std::vector<std::vector<unsigned int>>	m_FaceIndicesColors;		//indices in color array (if size==0, indicesVertices is used)

private:
	inline vec3i toVirtualVoxelPos(const point3d<FloatType>& v, FloatType voxelSize) {
		return vec3i(v/voxelSize+(FloatType)0.5*point3d<FloatType>(math::sign(v)));
	} 
	//! returns -1 if there is no vertex closer to 'v' than thresh; otherwise the vertex id of the closer vertex is returned
	unsigned int hasNearestNeighbor(const vec3i& coord, SparseGrid3D<std::list<std::pair<point3d<FloatType>,unsigned int>>> &neighborQuery, const point3d<FloatType>& v, FloatType thresh );

	//! returns -1 if there is no vertex closer to 'v' than thresh; otherwise the vertex id of the closer vertex is returned (manhattan distance)
	unsigned int hasNearestNeighborApprox(const vec3i& coord, SparseGrid3D<unsigned int> &neighborQuery, FloatType thresh );
};

typedef MeshData<float>		MeshDataf;
typedef MeshData<double>	MeshDatad;


} // namespace ml

#include "meshData.cpp"

#endif  // CORE_MESH_MESHDATA_H_

