#include <vector>
#include <cassert>



// 2D array for storing a field variable
// in row-order => inner-most loop over j, nit i!
// Example:
//		Array2D u(N+1, N+1);
//		u(i, j) = 10.0;
class Array2D {
public:
		Array2D(int isize, int jsize) : data_(isize * jsize, 0.0), isize_(isize), jsize_(jsize) {
			assert(isize >= 0 && jsize >= 0);
		}

		const double& operator()(int i, int j) const {
			assert(i >= 0 && i < isize_ && j >= 0 && j < jsize_);
			return data_[i * jsize_ + j];
		}

		double& operator()(int i, int j) {
			assert(i >= 0 && i < isize_ && j >= 0 && j < jsize_);
			return data_[i * jsize_ + j];
		}

		int get_isize() {
			return isize_;
		}

		int get_jsize() {
			return jsize_;
		}

private:
		std::vector<double> data_;
		int isize_, jsize_;
};


// Writes a StructuredGrid files in the VTK format and summarise them in as a PVD file for ParaView
class VTKWriter {

public:
	VTKWriter(std::string file_path_prefix) : file_path_prefix_(file_path_prefix), steps_() {}

	bool write_step(Array2D u, int step_number, int domain_id = 0, double x_offset = 0.0, double y_offset = 0.0) {
		
		std::string file_path = file_path_prefix_ + std::to_string(step_number) + std::string("_") + std::to_string(domain_id) + std::string(".vts");


		FILE * out = fopen(file_path.c_str(), "w");
		if (out == NULL) {
			return false;
		}

		fprintf(out, "<VTKFile type = \"StructuredGrid\" version = \"1.0\" byte_order = \"LittleEndian\" header_type = \"UInt64\">\n");
		fprintf(out, "  <StructuredGrid WholeExtent=\"%d %d %d %d 0 0\">\n", 0, u.get_jsize() - 1, 0, u.get_isize() - 1);
		fprintf(out, "  <Piece Extent=\"%d %d %d %d 0 0\">\n",  0, u.get_jsize() - 1, 0, u.get_isize() - 1);
		fprintf(out, "    <Points>\n");
		fprintf(out, "      <DataArray type=\"Float32\" Name=\"Points\" NumberOfComponents=\"3\" format=\"ascii\">\n");
		for (int i = 0; i < u.get_isize(); i++) {
		       for (int j = 0; j < u.get_jsize(); j++) {
		              fprintf(out, "%f %f %f ", (double)j, (double)i, 0.0);
		       }
		}
		fprintf(out, "\n      </DataArray>\n");
		fprintf(out, "    </Points>\n");
		fprintf(out, "    <PointData Scalars=\"u\">\n");
		fprintf(out, "      <DataArray type=\"Float32\" Name=\"u\" format=\"ascii\">\n");
		for (int i = 0; i < u.get_isize(); i++) {
		    for (int j = 0; j < u.get_jsize(); j++) {
		    	fprintf(out, "%f ", u(i,j));
		    }
		}
		fprintf(out, "\n      </DataArray>\n");
		fprintf(out, "    </PointData>\n");
		fprintf(out, "    <CellData> \n </CellData>\n");
		fprintf(out, "  </Piece>\n");
		fprintf(out, "</StructuredGrid>\n");
		fprintf(out, "</VTKFile>\n");


		fclose(out);

		steps_.push_back(step_number);
		return true;
	}

	bool finalize(int domains = 0) {

		std::string pvd_file_name = file_path_prefix_ + std::string(".pvd");
        FILE* out = fopen(pvd_file_name.c_str(), "w");

		if (out == NULL) {
			return false;
		}

        fprintf(out, "<?xml version=\"1.0\"?>\n");
        fprintf(out, "<VTKFile type=\"Collection\" version=\"0.1\">\n");
        fprintf(out, "  <Collection>>\n");

        for (auto step_number : steps_) {
        	for (int domain_id = 0; domain_id <= domains; domain_id++) {
				std::string file = file_path_prefix_ + std::to_string(step_number) + std::string("_") + std::to_string(domain_id) + std::string(".vts");

				fprintf(out, "    <DataSet timestep=\"%d\" group=\"\" part=\"0\"\n", step_number);
				fprintf(out, "             file=\"%s\"/>\n", file.c_str());
			}
		}
        fprintf(out, "  </Collection>\n");
        fprintf(out, "</VTKFile>");

        fclose(out);
		return true;
	}

private:
	std::string file_path_prefix_; 
	std::vector<int> steps_;



};
