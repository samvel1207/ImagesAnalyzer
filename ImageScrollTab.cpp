
#include <QLabel>
#include <QFileInfo>
#include <QVBoxLayout>

#include "ImageScrollTab.h"
#include "ImageScrollWidget.h"


ImageScrollTab::ImageScrollTab()
{
	descriptionLabel_ = new QLabel();

	imageScrollWidget_ = new ImageScrollWidget;

	connect( imageScrollWidget_, SIGNAL( imageChanged( const QFileInfo& ) ), 
			this, SLOT( updateDescriptionText( const QFileInfo& ) ) );

	QVBoxLayout* layout = new QVBoxLayout;
	layout->addWidget( descriptionLabel_ );
	layout->addSpacing( 10 );
	layout->addWidget( imageScrollWidget_, 1 );

	setLayout( layout );
}

void ImageScrollTab::updateDescriptionText( const QFileInfo& imageDir )
{
	//
	// ToDo Implement custom update of label's text here
	//

	descriptionLabel_->setText( imageDir.absoluteFilePath() );
}
