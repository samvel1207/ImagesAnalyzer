
#include <QLabel>
#include <QFileInfo>
#include <QVBoxLayout>

#include "ImageTab.h"

#include <cassert>


ImageTab::ImageTab()
{
	descriptionLabel_ = new QLabel();

	image_ = new QImage();
	imageWidget_ = new QLabel();
	imageWidget_->setScaledContents( true );
	imageWidget_->setMaximumSize( QSize( 1000, 600 ) );

	QVBoxLayout* layout = new QVBoxLayout;
	layout->addWidget( descriptionLabel_ );
	layout->addSpacing( 10 );
	layout->addWidget( imageWidget_, 1 );

	setLayout( layout );
}

void ImageTab::loadImage( const QFileInfo& imagePath )
{
	assert( imagePath.isFile() );
	image_->load( imagePath.absoluteFilePath() );
	imageWidget_->setPixmap( QPixmap::fromImage( *image_ ) );
	updateDescriptionText( imagePath );
}

void ImageTab::clean()
{
	image_->load( "" );
	imageWidget_->setPixmap( QPixmap::fromImage( *image_ ) );
	updateDescriptionText( QFileInfo() );
}

void ImageTab::updateDescriptionText( const QFileInfo& imageDir )
{
	//
	// ToDo Implement custom behavior here
	//

	descriptionLabel_->setText( imageDir.absoluteFilePath() );
}
