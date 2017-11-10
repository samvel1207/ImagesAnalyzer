#include "images_processor.h"
#include <QImage>

void ImagesProcessor::process_dir_template(const QString& images_dir_path, const QString& template_path)
{
	std::list<QImage> images_list;
	load_dir_images(images_dir_path, &images_list);
}

void ImagesProcessor::process_map_template(const QString& map_image_path, const QString& template_path)
{
	QImage map_image(map_image_path);
	QImage temp_image(template_path);

	int w=map_image.width();
	int h=map_image.height();
	double max_w2=0;
	double max_x=0, max_y=0;

	StatParams param1=getImageStats(temp_image);
	ImageWeibulStats w_stats1;
	w_stats1.c=searchC(param1);
	w_stats1.b=param1.E_X/gammaFunc(1+1.0/w_stats1.c);

	int **maskR = createBitmap(w, h, int(0));
	int **maskG = createBitmap(w, h, int(0));
	int **maskB = createBitmap(w, h, int(0));
	createRGBMask(map_image, maskR, maskG, maskB);

	std::pair<double**, double**> sobelMask=createSobelMask(map_image);
	int cw=temp_image.width()-2;
	int ch=temp_image.height()-2;
	double m_size2=cw*ch;
	int m_width = cw+1;
	int m_height = ch+1;
	for (int i=1; i<w-m_width; ++i) {
		for (int j=1; j<h-m_height; ++j) {
			StatParams param2;
			param2.E_X = (sobelMask.first[i+cw][j+ch]-sobelMask.first[i][j+ch]-sobelMask.first[i+cw][j]+sobelMask.first[i][j])/m_size2;
			param2.D_X = (sobelMask.second[i+cw][j+ch]-sobelMask.second[i][j+ch]-sobelMask.second[i+cw][j]+sobelMask.second[i][j])/m_size2-param2.E_X*param2.E_X;
			ImageWeibulStats w_stats2;
			w_stats2.c=searchC(param2);
			w_stats2.b=param2.E_X/gammaFunc(1+1.0/w_stats2.c);

			int tmp_r=(maskR[i+m_width][j+m_height]-maskR[i+m_width][j]-maskR[i][j+m_height]+maskR[i][j])/(m_width*m_height);
			int tmp_g=(maskG[i+m_width][j+m_height]-maskG[i+m_width][j]-maskG[i][j+m_height]+maskG[i][j])/(m_width*m_height);
			int tmp_b=(maskB[i+m_width][j+m_height]-maskB[i+m_width][j]-maskB[i][j+m_height]+maskB[i][j])/(m_width*m_height);
			
			double tmp_w2=calculateW2(w_stats1, w_stats2);
			if (tmp_w2>max_w2) {
				max_x=i;
				max_y=j;
				max_w2=tmp_w2;
			}	
		}
	}
	deleteBitmap(sobelMask.first, w);
	deleteBitmap(sobelMask.second, w);
	deleteBitmap(maskR, w);
	deleteBitmap(maskG, w);
	deleteBitmap(maskB, w);
	draw_template_answer(map_image, m_width, m_height, max_x, max_y);
	QString file_path = results_dir_;
	file_path.append("/search_result.jpg");
	map_image.save(file_path);
}

void ImagesProcessor::draw_template_answer(QImage& image, int w, int h, int x, int y)
{
	for (int i=0; i<w; ++i) {
		image.setPixel(x+i, y, qRgb(255, 0, 0));
		image.setPixel(x+i, y+1, qRgb(255, 0, 0));
		image.setPixel(x+i, y+h-1, qRgb(255, 0, 0));
		image.setPixel(x+i, y+h, qRgb(255, 0, 0));
	}
	for (int i=0; i<h; ++i) {
		image.setPixel(x, y+i, qRgb(255, 0, 0));
		image.setPixel(x+1, y+i, qRgb(255, 0, 0));
		image.setPixel(x+w-1, y+i, qRgb(255, 0, 0));
		image.setPixel(x+w, y+i, qRgb(255, 0, 0));
	}
}

void ImagesProcessor::process_dir_pattern(const QString& images_dir_path, SpecialPattern pattern)
{
	std::list<QImage> images_list;
	load_dir_images(images_dir_path, &images_list);
	int num=1;
	for (std::list<QImage>::const_iterator it=images_list.begin(); it!=images_list.end(); ++it, ++num) {
		QImage new_image(*it);
		new_image.convertToFormat(QImage::Format_ARGB32);
		int length = new_image.width();
		int height = new_image.height();
		const int k = 3;
		int lk = length/k;
		int hk = height/k;
		int sum_length = 0;
		int sum_height = 0;
		int counter = 0;
		for (int i=0; i<length; ++i) {
			for (int j=0; j<height; ++j) {
				QRgb pixel = new_image.pixel(i, j);
				if (is_searched_pixel(qRed(pixel), qGreen(pixel), qBlue(pixel), pattern)) {
					sum_length+=i;
					sum_height+=j;
					++counter;
				}
			}
		}
		if (counter > length*height/1000) {
			int center_l=sum_length/counter;
			int center_h=sum_height/counter;
			for (int l=center_l-10>0?center_l-10:0; l<center_l+10 && l<length; ++l)
				for (int h=center_h-10>0?center_h-10:0; h<center_h+10 && h<height; ++h) {
					new_image.setPixel(l, h, qRgb(255, 0, 0));
				}
		}
		for (int i=1; i<k; ++i) {
			for (int j=0; j<height; ++j)
				new_image.setPixel(i*lk, j, qRgb(0, 255, 0));
			for (int j=0; j<length; ++j)
				new_image.setPixel(j, i*hk, qRgb(0, 255, 0));
		}
		QString file_path = results_dir_;
		file_path.append("/");
		file_path.append(QString::number(num));
		file_path.append(".jpg");
		new_image.save(file_path);
	}
}

void ImagesProcessor::draw_path(QImage& image, int& prev_x, int& prev_y, int x, int y)
{
	if (prev_y==y) {
		for (int i=prev_x; i<x; ++i) {
			image.setPixel(i, y, qRgb(255, 0, 0));
			image.setPixel(i, y-1, qRgb(255, 0, 0));
			image.setPixel(i, y+1, qRgb(255, 0, 0));
		}
		for (int i=prev_x; i>x; --i) {
			image.setPixel(i, y, qRgb(255, 0, 0));
			image.setPixel(i, y-1, qRgb(255, 0, 0));
			image.setPixel(i, y+1, qRgb(255, 0, 0));
		}
	}
	else if (prev_x==x) {
		for (int i=prev_y; i<y; ++i) {
			image.setPixel(x, i, qRgb(255, 0, 0));
			image.setPixel(x-1, i, qRgb(255, 0, 0));
			image.setPixel(x+1, i, qRgb(255, 0, 0));
		}
	}
	else {
		double k=(double)(y-prev_y)/(double)(x-prev_x);
		for (int i=prev_x; i<x; ++i) {
			image.setPixel(i, k*(i-prev_x)+prev_y, qRgb(255, 0, 0));
			image.setPixel(i, k*(i-prev_x)-1+prev_y, qRgb(255, 0, 0));
			image.setPixel(i, k*(i-prev_x)+1+prev_y, qRgb(255, 0, 0));
		}
		for (int i=prev_x; i>x; --i) {
			image.setPixel(i, k*(i-prev_x)+prev_y, qRgb(255, 0, 0));
			image.setPixel(i-1, k*(i-prev_x)-1+prev_y, qRgb(255, 0, 0));
			image.setPixel(i+1, k*(i-prev_x)+1+prev_y, qRgb(255, 0, 0));
		}
	}
	prev_x=x;
	prev_y=y;
}

void ImagesProcessor::process_map_pattern(const QString& map_image_path, SpecialPattern pattern)
{
	QImage map_image = QImage(map_image_path);
	QImage path_image(map_image);
	int h = map_image.height(), w = map_image.width();
	int h1 = h/5, w1 = w/5;
	int dh = h1/3, dw=w1/3;
	int search_section=0;
	int num=0;
	int prev_x=w1/2, prev_y=h1/2, x0, y0;
	for (int i=0; i<=h-h1 && search_section==0; i+=dh) {
		if ((i/dh)%2==0) {
			for (int j=0; j<=w-w1 && search_section==0; j+=dw) {
				++num;
				QRect rect(j, i, w1, h1);
				QImage cropped = map_image.copy(rect);
				search_section = process_cropped(cropped, pattern);
				x0=j, y0=i;
				dump_cropped(map_image, cropped, num, x0, y0, x0+w1, y0+h1, search_section);
				draw_path(path_image, prev_x, prev_y, x0+w1/2, y0+h1/2);
			}
		}
		else {
			for (int j=(w-w1)/dw*dw; j>=0 && search_section==0; j-=dw) {
				++num;
				QRect rect(j, i, w1, h1);
				QImage cropped = map_image.copy(rect);
				search_section = process_cropped(cropped, pattern);
				x0=j, y0=i;
				dump_cropped(map_image, cropped, num, x0, y0, x0+w1, y0+h1, search_section);
				draw_path(path_image, prev_x, prev_y, x0+w1/2, y0+h1/2);
			}
		}
	}
	QString file_path = results_dir_;
	file_path.append("/search_route.jpg");
	path_image.save(file_path);
}

void ImagesProcessor::dump_cropped(QImage& map_image, QImage& cropped, int num, int& x0, int& y0, int x1, int y1, int search_section)
{
	QString file_path = results_dir_;
	file_path.append("/");
	file_path.append(QString::number(num));
	file_path.append(".jpg");
	int h=cropped.height(), w=cropped.width();
	for (int i=1; i<3; ++i) {
		for (int j=0; j<w; ++j)
			cropped.setPixel(j, i*h/3, qRgb(0, 255, 0));
		for (int j=0; j<h; ++j)
			cropped.setPixel(i*w/3, j, qRgb(0, 255, 0));
	}
	cropped.save(file_path);
	int dx=(x1-x0)/3, dy=(y1-y0)/3;
	if (search_section != 0) {
		if (search_section == 1) {
			x0-=dx, y0-=dy;
			x1-=dx, y1-=dy;
		} else if (search_section == 2) {
			y0-=dy;
			y1-=dy;
		} else if (search_section == 3) {
			x0+=dx, y0-=dy;
			x1+=dx, y1-=dy;
		} else if (search_section == 4) {
			x0-=dx;
			x1-=dx;
		} else if (search_section == 6) {
			x0+=dx;
			x1+=dx;
		} else if (search_section == 7) {
			x0-=dx, y0+=dy;
			x1-=dx, y1+=dy;
		} else if (search_section == 8) {
			y0+=dy;
			y1+=dy;
		} else if (search_section == 9) {
			x0+=dx, y0+=dy;
			x1+=dx, y1+=dy;
		}
		file_path = results_dir_;
		file_path.append("/");
		file_path.append(QString::number(++num));
		file_path.append(".jpg");
		QRect rect(x0, y0, x1-x0, y1-y0);
		QImage res = map_image.copy(rect);
		for (int i=1; i<3; ++i) {
			for (int j=0; j<w; ++j)
				res.setPixel(j, i*h/3, qRgb(0, 255, 0));
			for (int j=0; j<h; ++j)
				res.setPixel(i*w/3, j, qRgb(0, 255, 0));
		}
		res.save(file_path);
	}
}

int ImagesProcessor::process_cropped(const QImage& cropped, SpecialPattern pattern)
{
	int h = cropped.height(), w = cropped.width();
	int dh = h/3, dw=w/3;
	int counter = 0;
	int sum_w = 0;
	int sum_h = 0;
	int k=0;
	for (; k<9; ++k) {
		int ki = k/3*dh, kj = k%3*dw, counterk=0;
		for (int i=0; i<dh; ++i) {
			for (int j=0; j<dw; ++j) {
				QRgb pixel = cropped.pixel(j+kj, i+ki);
				if (is_searched_pixel(qRed(pixel), qGreen(pixel), qBlue(pixel), pattern)) {
					sum_w+=j+kj;
					sum_h+=i+ki;
					++counter;
					++counterk;
				}
			}
		}
		if (counterk > dw*dh/5) {
			return k+1;
		}
	}
	return 0;
}

bool ImagesProcessor::is_searched_pixel(int r, int g, int b, SpecialPattern pattern)
{
	if (pattern == FIRE) {
		int mn = r<g?r:g;
		mn = mn<b?mn:b;
		int mx = r>g?r:g;
		mx = mx>b?mx:b;
		return (r>160 && g>80 && g<180 && b<100) || (r>240 && g>200 && b<80 && r>g);
	}
	else if (pattern == SMOKE) {
		int mn = r<g?r:g;
		mn = mn<b?mn:b;
		int mx = r>g?r:g;
		mx = mx>b?mx:b;		
		return (r>90 && r<190 && g>130 && g<170 && b>90 && b<190 && mx-mn<30);
	}
	else if (pattern == ROAD) {
		process_road_pattern();
	}
	return false;
}

void ImagesProcessor::load_dir_images(const QString& images_dir_path, std::list<QImage>* images_list)
{
	QDir dir(images_dir_path);
	QStringList filter;
	filter << QLatin1String("*.png") << QLatin1String("*.jpeg") << QLatin1String("*.jpg") << QLatin1String("*.bmp") << QLatin1String("*.bmp") << QLatin1String("*.gif");
	dir.setNameFilters(filter);

	QFileInfoList file_list_info = dir.entryInfoList();
	QStringList fileList;
	if (file_list_info.empty()) {
		std::string error_msg="There are no images in ";
		error_msg.append(images_dir_path.toLatin1().data());
		throw std::exception(error_msg.c_str());
	}
	foreach (const QFileInfo &file_info, file_list_info) {
		QString image_file = file_info.absoluteFilePath();
		images_list->push_back(QImage(image_file));
	}
}

void ImagesProcessor::process_fire_pattern()
{

}

void ImagesProcessor::process_smoke_pattern()
{

}

void ImagesProcessor::process_road_pattern()
{

}

double ImagesProcessor::calculateW2(const ImageWeibulStats& stat1, const ImageWeibulStats& stat2)
{
	double min_c, min_b, max_c, max_b;
	if (stat1.c<stat2.c) {
		min_c=stat1.c;
		max_c=stat2.c;
	}
	else {
		min_c=stat2.c;
		max_c=stat1.c;
	}
	if (stat1.b<stat2.b) {
		min_b=stat1.b;
		max_b=stat2.b;
	}
	else {
		min_b=stat2.b;
		max_b=stat1.b;
	}
	return min_c*min_b/(max_c*max_b);
}
double ImagesProcessor::gammaFunc(double x)
{
	if (x<1)
		return -1;
	if (x>2)
		return (x-1)*gammaFunc(x-1);
	double result=1;
	double xn=(x-1);
	for (int i=0; i<5; ++i) {
		result+=arr_[i]*xn;
		xn*=(x-1);
	}
	return result;
}
double ImagesProcessor::searchC(StatParams &param)
{
	double l=0.2, r=10.0;
	double g1, g2, c;
	double ans=param.D_X/(param.E_X*param.E_X)+1;
	double tmp_ans;
	while(r-l>0.001) {
		c=(l+r)/2;
		g1=gammaFunc(1+2.0/c);
		g2=gammaFunc(1+1.0/c);
		tmp_ans=g1/(g2*g2);
		if (ans>tmp_ans)
			r=c;
		else
			l=c;
	}
	return c;
}
template<class T>
T** ImagesProcessor::createBitmap(int w, int h, T type)
{
	T **bitmap = new T*[w];
	for (int i=0; i<w; ++i)
		bitmap[i] = new T[h];
	return bitmap;
}
template<class T>
void ImagesProcessor::deleteBitmap(T **bitmap, int w)
{
	if (bitmap != NULL) {
		for (int i=0; i<w; ++i)
			delete [] bitmap[i];
		delete [] bitmap;
	}
}
ImagesProcessor::StatParams ImagesProcessor::getImageStats(const QImage& image)
{
	StatParams result;
	int w=image.width();
	int h=image.height();
	double sumEX=0, sumEX2=0;
	int gray;
	int **bits = createBitmap(w, h, int(0));
	for (int i=0; i<w; ++i) {
		for (int j=0; j<h; ++j) {
			bits[i][j] = qGray(image.pixel(i, j));
		}
	}
	int Gx[3][3]={-1,0,1, -2,0,2, -1,0,1};
	int Gy[3][3]={1,2,1, 0,0,0, -1,-2,-1};
	for (int i=1; i<w-1; ++i) {
		for (int j=1; j<h-1; ++j) {
			int xGr = Gx[0][0]*bits[i-1][j-1]+Gx[0][1]*bits[i-1][j]+Gx[0][2]*bits[i-1][j+1]+
				Gx[1][0]*bits[i  ][j-1]+Gx[1][1]*bits[i  ][j]+Gx[1][2]*bits[i  ][j+1]+
				Gx[2][0]*bits[i+1][j-1]+Gx[2][1]*bits[i+1][j]+Gx[2][2]*bits[i+1][j+1];
			int yGr = Gy[0][0]*bits[i-1][j-1]+Gy[0][1]*bits[i-1][j]+Gy[0][2]*bits[i-1][j+1]+
				Gy[1][0]*bits[i  ][j-1]+Gy[1][1]*bits[i  ][j]+Gy[1][2]*bits[i  ][j+1]+
				Gy[2][0]*bits[i+1][j-1]+Gy[2][1]*bits[i+1][j]+Gy[2][2]*bits[i+1][j+1];
			double gr=sqrt((double)(xGr*xGr+yGr*yGr));
			sumEX += gr;
			sumEX2 += gr*gr;
		}
	}
	result.E_X=sumEX/(double)((w-2)*(h-2));
	result.D_X=sumEX2/(double)((w-2)*(h-2))-result.E_X*result.E_X;
	deleteBitmap(bits, w);
	return result;
}
void ImagesProcessor::createRGBMask(const QImage& image, int** maskR, int** maskG, int** maskB)
{
	int w=image.width();
	int h=image.height();

	QRgb pixel = image.pixel(0, 0);
	maskR[0][0] = qRed(pixel);
	maskG[0][0] = qGreen(pixel);
	maskB[0][0] = qBlue(pixel);
	for (int i=1; i<w; ++i) {
		pixel = image.pixel(i, 0);
		maskR[i][0] = qRed(pixel)+maskR[i-1][0];
		maskG[i][0] = qGreen(pixel)+maskG[i-1][0];
		maskB[i][0] = qBlue(pixel)+maskB[i-1][0];
	}
	for (int i=1; i<h; ++i) {
		pixel = image.pixel(0, i);
		maskR[0][i] = qRed(pixel)+maskR[0][i-1];
		maskG[0][i] = qGreen(pixel)+maskG[0][i-1];
		maskB[0][i] = qBlue(pixel)+maskB[0][i-1];
	}
	for (int i=1; i<w; ++i) {
		for (int j=1; j<h; ++j) {
			pixel = image.pixel(i, j);
			maskR[i][j] = qRed(pixel)+maskR[i-1][j]+maskR[i][j-1]-maskR[i-1][j-1];
			maskG[i][j] = qGreen(pixel)+maskG[i-1][j]+maskG[i][j-1]-maskG[i-1][j-1];
			maskB[i][j] = qBlue(pixel)+maskB[i-1][j]+maskB[i][j-1]-maskB[i-1][j-1];
		}
	}
}
std::pair<double**, double**> ImagesProcessor::createSobelMask(const QImage& image)
{
	int w=image.width();
	int h=image.height();
	int **bits = createBitmap(w, h, int(0));
	double **sobelBits = createBitmap(w, h, double(0.0));

	for (int i=0; i<w; ++i) {
		for (int j=0; j<h; ++j) {
			bits[i][j] = qGray(image.pixel(i, j));
		}
	}
	int Gx[3][3]={-1,0,1, -2,0,2, -1,0,1};
	int Gy[3][3]={1,2,1, 0,0,0, -1,-2,-1};
	for (int i=1; i<w-1; ++i) {
		for (int j=1; j<h-1; ++j) {
			int xGr = Gx[0][0]*bits[i-1][j-1]+Gx[0][1]*bits[i-1][j]+Gx[0][2]*bits[i-1][j+1]+
				Gx[1][0]*bits[i  ][j-1]+Gx[1][1]*bits[i  ][j]+Gx[1][2]*bits[i  ][j+1]+
				Gx[2][0]*bits[i+1][j-1]+Gx[2][1]*bits[i+1][j]+Gx[2][2]*bits[i+1][j+1];
			int yGr = Gy[0][0]*bits[i-1][j-1]+Gy[0][1]*bits[i-1][j]+Gy[0][2]*bits[i-1][j+1]+
				Gy[1][0]*bits[i  ][j-1]+Gy[1][1]*bits[i  ][j]+Gy[1][2]*bits[i  ][j+1]+
				Gy[2][0]*bits[i+1][j-1]+Gy[2][1]*bits[i+1][j]+Gy[2][2]*bits[i+1][j+1];
			sobelBits[i][j]=sqrt((double)(xGr*xGr+yGr*yGr));
		}
	}

	double **sobelMaskE = createBitmap(w, h, double(0.0));
	double **sobelMaskE2 = createBitmap(w, h, double(0.0));
	for (int i=0; i<w; ++i) {
		sobelMaskE[i][0]=0.0;
		sobelMaskE2[i][0]=0.0;
	}
	for (int j=0; j<h; ++j) {
		sobelMaskE[0][j]=0.0;
		sobelMaskE2[0][j]=0.0;
	}
	for (int i=1; i<w-1; ++i) {
		for (int j=1; j<h-1; ++j) {
			sobelMaskE[i][j]=sobelBits[i][j]+sobelMaskE[i][j-1]+sobelMaskE[i-1][j]-sobelMaskE[i-1][j-1];
			sobelMaskE2[i][j]=sobelBits[i][j]*sobelBits[i][j]+sobelMaskE2[i][j-1]+sobelMaskE2[i-1][j]-sobelMaskE2[i-1][j-1];
		}
	}

	// We do not delete sobelMaskE and sobelMaskE2 as they are being deleted in searchW2 method
	deleteBitmap(sobelBits, w);
	deleteBitmap(bits, w);
	return std::make_pair(sobelMaskE, sobelMaskE2);
}
