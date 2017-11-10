#pragma once

#include <QDir>

class ImagesProcessor
{
public:
	enum SpecialPattern
	{
		NONE	= 0,
		FIRE	= 1,
		SMOKE	= 2,
		ROAD	= 3
	};
	struct StatParams
	{
		double E_X;
		double D_X;
	};
	struct ImageWeibulStats
	{
		double b;
		double c;
	};

private:
	QString results_dir_;
	double arr_[6];

	void process_fire_pattern();
	void process_smoke_pattern();
	void process_road_pattern();
	bool is_searched_pixel(int r, int g, int b, SpecialPattern pattern);
	int process_cropped(const QImage& cropped, SpecialPattern pattern);
	void dump_cropped(QImage& map_image, QImage& cropped, int num, int& x0, int& y0, int x1, int y1, int search_section);
	void draw_path(QImage& image, int& prev_x, int& prev_y, int x, int y);
	void draw_template_answer(QImage& image, int w, int h, int x, int y);
	void load_dir_images(const QString& images_dir_path, std::list<QImage>* images_list);

	//functions for template matching
	double gammaFunc(double x);
	double calculateW2(const ImageWeibulStats& stat1, const ImageWeibulStats& stat2);
	double searchC(StatParams &param);
	void searchW2();
	template<class T>
	T** createBitmap(int w, int h, T type);
	template<class T>
	void deleteBitmap(T **bitmap, int w);
	StatParams getImageStats(const QImage& image);
	void createRGBMask(const QImage& image, int** maskR, int** maskG, int** maskB);
	std::pair<double**, double**> createSobelMask(const QImage& image);

public:
	ImagesProcessor(const QString& results_dir_path):results_dir_(results_dir_path)
	{
		results_dir_ += "/results";
		QDir().mkdir(results_dir_);

		// special values to calculate the gamma function
		arr_[0]=-0.5748646;
		arr_[1]=0.4245549;
		arr_[2]=0.9512363;
		arr_[3]=-0.1010678;
		arr_[4]=-0.6998588;
	}

	void process_dir_template(const QString& images_dir_path, const QString& template_path);
	void process_map_template(const QString& map_image_path, const QString& template_path);
	void process_dir_pattern(const QString& images_dir_path, SpecialPattern pattern);
	void process_map_pattern(const QString& map_image_path, SpecialPattern pattern);
};
