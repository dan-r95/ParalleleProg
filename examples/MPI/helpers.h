#include <vector>
#include <cassert>



// 2D domain for storing a field variable
// in row-order => inner-most loop over j, not i!
// Example:
//		Domain2D u(N+1, N+1);
//		u(i, j) = 10.0;
// Notes:
//		Assumes equal sized domains (isize, jsize) in a multi-domain setting
class Domain2D {
public:
		Domain2D(int isize, int jsize, int coord_i = 0, int coord_j = 0) 
		: 	data_(isize * jsize, 0.0), 
			isize_(isize), 
			jsize_(jsize),
			domain_coord_i_(coord_i),
			domain_coord_j_(coord_j) {
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

		int get_domain_coord_i() {
			return domain_coord_i_;
		}

		int get_domain_coord_j() {
			return domain_coord_j_;
		}

		int get_min_x() {

			return domain_coord_i_ * (isize_ - 2);
		}

		int get_max_x() {
			return this->get_min_x() + isize_ - 1;
		}

		int get_min_y() {
			return domain_coord_j_ * (jsize_ - 2);
		}

		int get_max_y() {
			return this->get_min_y() + jsize_ - 1;
		}

		double get_x(int i) {
			return static_cast<double>(this->get_min_x() + i);
		}

		double get_y(int j) {
			return static_cast<double>(this->get_min_y() + j);
		}


private:
		std::vector<double> data_;
		int isize_, jsize_, domain_coord_i_, domain_coord_j_;
};


// Writes a StructuredGrid files in the VTK format and summarise them in as a PVD file for ParaView
class VTKWriter {

public:
	VTKWriter(std::string file_path_prefix) : file_path_prefix_(file_path_prefix), steps_() {}

	bool write_step(Domain2D u, int step_number) {
		
		std::string file_path = file_path_prefix_ + std::to_string(step_number) + 
								std::string("_") + std::to_string(u.get_domain_coord_i()) + 
								std::string("_") + std::to_string(u.get_domain_coord_j()) + 
								std::string(".vts");


		FILE * out = fopen(file_path.c_str(), "w");
		if (out == NULL) {
			return false;
		}

		fprintf(out, "<VTKFile type = \"StructuredGrid\" version = \"1.0\" byte_order = \"LittleEndian\" header_type = \"UInt64\">\n");
		fprintf(out, "  <StructuredGrid WholeExtent=\"%d %d %d %d 0 0\">\n", u.get_min_x(), u.get_max_x(), u.get_min_y(), u.get_max_y());
		fprintf(out, "  <Piece Extent=\"%d %d %d %d 0 0\">\n", u.get_min_x(), u.get_max_x(), u.get_min_y(), u.get_max_y());
		fprintf(out, "    <Points>\n");
		fprintf(out, "      <DataArray type=\"Float32\" Name=\"Points\" NumberOfComponents=\"3\" format=\"ascii\">\n");
		for (int j = 0; j < u.get_jsize(); j++) {
			for (int i = 0; i < u.get_isize(); i++) {
		              fprintf(out, "%f %f %f ", u.get_x(i), u.get_y(j), 0.0);
		       }
		}
		fprintf(out, "\n      </DataArray>\n");
		fprintf(out, "    </Points>\n");
		fprintf(out, "    <PointData Scalars=\"u\">\n");
		fprintf(out, "      <DataArray type=\"Float32\" Name=\"u\" format=\"ascii\">\n");
		for (int j = 0; j < u.get_jsize(); j++) {
			for (int i = 0; i < u.get_isize(); i++) {
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

	bool finalize_step(int step_number, int isize, int jsize, int num_dims_i = 0, int num_dims_j = 0) {

		std::string file_path = file_path_prefix_ + std::to_string(step_number) + std::string(".pvts");

		FILE * out = fopen(file_path.c_str(), "w");
		if (out == NULL) {
			return false;
		}

		fprintf(out, "<VTKFile type = \"PStructuredGrid\">\n");
		fprintf(out, "  <PStructuredGrid WholeExtent=\"%d %d %d %d 0 0\" GhostLevel=\"1\">\n", 0, (isize - 2) * num_dims_i + 1, 0, (jsize - 2) * num_dims_j + 1);
    	fprintf(out, "    <PPointData Scalars=\"u\">\n");
		fprintf(out, "      <DataArray type=\"Float32\" Name=\"u\" format=\"ascii\"></DataArray>\n");
    	fprintf(out, "    </PPointData>\n");
    	fprintf(out, "    <PPoints>\n");
		fprintf(out, "      <DataArray type=\"Float32\" Name=\"Points\" NumberOfComponents=\"3\" format=\"ascii\"></DataArray>\n");
    	fprintf(out, "    </PPoints>\n");

		for (int j = 0; j < num_dims_j; j++) {
			for (int i = 0; i < num_dims_i; i++) {
				std::string file = file_path_prefix_ + std::to_string(step_number) + 
									std::string("_") + std::to_string(i) + 
									std::string("_") + std::to_string(j) + std::string(".vts");
				int x_min = i * (isize - 2);
				int x_max = i * (isize - 2) + isize - 1;
				int y_min = j * (jsize - 2);
				int y_max = j * (jsize - 2) + jsize - 1;
				fprintf(out, "    <Piece Extent=\"%d %d %d %d 0 0\" Source=\"%s\"/>\n", x_min, x_max, y_min, y_max,  file.c_str());
			}
		}
		fprintf(out, "</PStructuredGrid>\n");
		fprintf(out, "</VTKFile>\n");

		fclose(out);

		return true;
	}

	bool finalize() {

		std::string pvd_file_name = file_path_prefix_ + std::string(".pvd");
        FILE* out = fopen(pvd_file_name.c_str(), "w");

		if (out == NULL) {
			return false;
		}

        fprintf(out, "<?xml version=\"1.0\"?>\n");
        fprintf(out, "<VTKFile type=\"Collection\" version=\"0.1\">\n");
        fprintf(out, "  <Collection>>\n");

        for (auto step_number : steps_) {
			fprintf(out, "    <DataSet timestep=\"%d\" group=\"\" part=\"0\"\n", step_number);
			std::string file = file_path_prefix_ + std::to_string(step_number) + std::string(".pvts");
			fprintf(out, "             file=\"%s\"/>\n", file.c_str());
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
