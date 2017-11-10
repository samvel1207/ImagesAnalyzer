
#ifndef IMGPROC__IMAGE_SCROLL_WIDGET_H
#define IMGPROC__IMAGE_SCROLL_WIDGET_H

#include <QWidget>
#include <QFileInfo>

/// Forward declarations
class QDir;
class QLabel;
class QPushButton;
class QImage;


/**
 * This widget allows to scroll all iamges of some directory 
 * with help of 2 buttons ("left" and "right").
 */
class ImageScrollWidget : public QWidget
{
	Q_OBJECT

public:
	ImageScrollWidget( QWidget* parent = 0 );

	~ImageScrollWidget();

	/**
	 * Reloads all images paths from specified directory 'dir', and 
	 * displays the image with index 'displayIndex'.
	 */
	void loadFromDirectory( 
			const QDir& dir, 
			int displayIndex = 0 );

	/**
	 * Cleans content of this image scroll widget.
	 */
	void clean();

	/**
	 * Returns the number of iamges loaded.
	 */
	int imagesCount() const;

	/**
	 * Returns full path of the image with specified index. Pay attention 
	 * that indices start from 0.
	 */
	QFileInfo imageDir( int index ) const;

	/**
	 * Returns index of currently displayed image.
	 */
	int currentIndex() const;

	/**
	 * Returns full path of currently displayed image.
	 */
	QFileInfo currentImageDir() const;

public slots:
	/**
	 * Displays next image.
	 * @return If switch to next was performed (current image was not last).
	 */
	bool displayNext();

	/**
	 * Displays previous image.
	 * @return If switch to previous was performed (current image was not first).
	 */
	bool displayPrevious();

	/**
	 * Displays image with specified index.
	 * @return If that image was displayed (index was in range). Pay attention
	 * that indices start from 0.
	 */
	bool displayWithIndex( int index );

signals:
	/**
	 * This signal is emitted when a new image is displayed in the widget.
	 * If no image is displayed, the signal is emitted with 'imageDir' being empty.
	 */
	void imageChanged( const QFileInfo& imageDir );

protected:
	/**
	 * Reloads image according to member parameters of this object.
	 */
	void reloadImage();

protected:
	QFileInfoList images_;

	int currentIndex_;

	QLabel* imageLabel_;
	QImage* image_;
	QPushButton* previousButton_;
	QPushButton* nextButton_;


};


#endif // IMGPROC__IMAGE_SCROLL_WIDGET_H
