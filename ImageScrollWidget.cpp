
#include <QLabel>
#include <QPixmap>
#include <QImage>
#include <QDir>
#include <QPushButton>
#include <QHBoxLayout>

#include "ImageScrollWidget.h"

#include <cassert>


ImageScrollWidget::ImageScrollWidget( QWidget* parent )
		: QWidget( parent )
{
	currentIndex_ = 0;

	image_ = new QImage;

	imageLabel_ = new QLabel;
	imageLabel_->setScaledContents( true );
	imageLabel_->setMaximumSize( QSize( 1000, 600 ) );
	imageLabel_->setPixmap( QPixmap::fromImage( *image_ ) );

	previousButton_ = new QPushButton( "< Previous" );
	connect( previousButton_, SIGNAL( clicked( bool ) ), 
			this, SLOT( displayPrevious() ) );

	nextButton_ = new QPushButton( "Next >" );
	connect( nextButton_, SIGNAL( clicked( bool ) ), 
			this, SLOT( displayNext() ) );

	QHBoxLayout* layout = new QHBoxLayout();
	layout->addWidget( previousButton_ );
	layout->addWidget( imageLabel_, 1 );
	layout->addWidget( nextButton_ );

	setLayout( layout );
}

ImageScrollWidget::~ImageScrollWidget()
{
	//image_->deleteLater();
}

void ImageScrollWidget::loadFromDirectory( 
		const QDir& dir, 
		int displayIndex )
{
	assert( dir.exists() );
	QDir workDir( dir );
	workDir.setNameFilters( QStringList() 
			<< "*.bmp" << "*.jpg" << "*.jpeg" << "*.png" << "*.gif" );
	images_ = workDir.entryInfoList();
	currentIndex_ = displayIndex;
	reloadImage();
}

void ImageScrollWidget::clean()
{
	images_.clear();
	currentIndex_ = 0;
	reloadImage();
}

int ImageScrollWidget::imagesCount() const
{
	return images_.size();
}

QFileInfo ImageScrollWidget::imageDir( int index ) const
{
	return images_[ index ];
}

int ImageScrollWidget::currentIndex() const
{
	return currentIndex_;
}

QFileInfo ImageScrollWidget::currentImageDir() const
{
	return images_[ currentIndex_ ];
}

bool ImageScrollWidget::displayNext()
{
	if ( currentIndex_ >= imagesCount() - 1 )
		return false;
	++currentIndex_;
	reloadImage();
	return true;
}

bool ImageScrollWidget::displayPrevious()
{
	if ( currentIndex_ <= 0 )
		return false;
	--currentIndex_;
	reloadImage();
	return true;
}

bool ImageScrollWidget::displayWithIndex( int index )
{
	if ( ! (0 <= index && index < imagesCount() ) )
		return false;
	currentIndex_ = index;
	reloadImage();
	return true;
}

void ImageScrollWidget::reloadImage()
{
	// Update buttons state
	previousButton_->setEnabled( currentIndex_ > 0 );
	nextButton_->setEnabled( currentIndex_ < imagesCount() - 1 );
	// If not valid image index
	if ( currentIndex_ < 0 || currentIndex_ >= imagesCount() ) {
		// Display empty set
		image_->load( "" );
		imageLabel_->setPixmap( QPixmap::fromImage( *image_ ) );
		emit imageChanged( QFileInfo() );
		return;
	}
	// Load image
	image_->load( currentImageDir().absoluteFilePath() );
	imageLabel_->setPixmap( QPixmap::fromImage( *image_ ) );
	// Emit necessary signals
	emit imageChanged( currentImageDir() );
}
