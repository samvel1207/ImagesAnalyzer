
#ifndef IMGPROC__IMAGE_SCROLL_TAB_H
#define IMGPROC__IMAGE_SCROLL_TAB_H

#include <QWidget>

/// Forward declarations
class QLabel;
class QFileInfo;

class ImageScrollWidget;


/**
 * This widget is used to show a result as a "scrolling images" tab.
 */
class ImageScrollTab : public QWidget
{
	Q_OBJECT

public:
	ImageScrollTab();

	/**
	 * Returns description-label control.
	 */
	QLabel* descriptionLabel();

	/**
	 * Returns image-scrolling widget control.
	 */
	ImageScrollWidget* imageScrollWidget();

protected slots:
	/**
	 * This method is called when description text in this tab 
	 * must be updated.
	 */
	void updateDescriptionText( const QFileInfo& imageDir );

protected:
	QLabel* descriptionLabel_;
	ImageScrollWidget* imageScrollWidget_;
};


inline QLabel* ImageScrollTab::descriptionLabel()
{
	return descriptionLabel_;
}

inline ImageScrollWidget* ImageScrollTab::imageScrollWidget()
{
	return imageScrollWidget_;
}


#endif // IMGPROC__IMAGE_SCROLL_TAB_H
