
#ifndef IMGPROC__IMAGE_TAB_H
#define IMGPROC__IMAGE_TAB_H

#include <QWidget>

/// Forward declarations
class QLabel;
class QFileInfo;


/**
 * This widget is used to show a result as a "images" tab.
 */
class ImageTab : public QWidget
{
	Q_OBJECT

public:
	ImageTab();

	/**
	 * Loads the image from the specified path.
	 */
	void loadImage( const QFileInfo& imagePath );

	/**
	 * Cleans image area.
	 */
	void clean();

	/**
	 * Returns description-label control.
	 */
	QLabel* descriptionLabel();

	/**
	 * Returns image widget control.
	 */
	QLabel* imageWidget();

protected slots:
	/**
	 * This method is called when description text in this tab 
	 * must be updated.
	 */
	void updateDescriptionText( const QFileInfo& imageDir );

protected:
	QLabel* descriptionLabel_;
	QImage* image_;
	QLabel* imageWidget_;
};


inline QLabel* ImageTab::descriptionLabel()
{
	return descriptionLabel_;
}

inline QLabel* ImageTab::imageWidget()
{
	return imageWidget_;
}


#endif // IMGPROC__IMAGE_TAB_H
