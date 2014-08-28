#include "Float32AEWidget.h"

#include <QtGui/QHBoxLayout>
#include <QtGui/QPainter>
#include <QtGui/QMenu>
#include <QtGui/QScrollBar>
#include <QtGui/QDialog>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QSpinBox>
#include <QtGui/QListWidget>
#include <QtGui/QListWidgetItem>
#include <QtGui/QSizePolicy>

#include "macros.h"
#include "SpliceStandalone.h"

using namespace FabricSplice;

AEWidget * Float32AEWidget::create( FabricCore::RTVal param , QWidget* parent)
{
	return new Float32AEWidget(param ,parent);
}

Float32AEWidget::Float32AEWidget(FabricCore::RTVal param ,QWidget* parent)
    : AEWidget(param,parent)
{
	setLayout(createLabelControlLayout());

  m_listWidget = new QWidget();
  m_listWidget->setLayout(new QVBoxLayout(m_listWidget));
	layout()->addWidget(m_listWidget);

  m_validator = new DoubleValidator(this);

  setRTVal(param);
			
	// QSpacerItem * spacerItem = new QSpacerItem(20,1,QSizePolicy::Expanding , QSizePolicy::Minimum);
	// list->addItem(spacerItem);

}

FabricCore::RTVal Float32AEWidget::getValueArray()
{
  FabricCore::RTVal values;

  FABRIC_TRY_RETURN("Float32AEWidget::getValueArray", FabricCore::RTVal(), 

  	values = constructRTVal("Float32[]");
    values.setArraySize(m_widgets.size());
  	for (unsigned int i = 0; i < m_widgets.size(); ++i)
    {
      QString value = m_widgets[i]->text();
      values.setArrayElement(i, constructFloat32RTVal(value.toFloat()));
    }
  );
	return values;
}

void Float32AEWidget::setRTVal(FabricCore::RTVal param)
{
  FABRIC_TRY("Float32AEWidget::setRTVal",
  
    AEWidget::setRTVal(constructObjectRTVal("Float32Parameter", 1, &param));
    setValueArray(m_param.callMethod("Float32[]", "getValueArray", 0, 0));

  );
}

void Float32AEWidget::setValueArray(FabricCore::RTVal values)
{
  FABRIC_TRY("Float32AEWidget::setValueArray", 

    unsigned int precision = m_param.callMethod("UInt32", "getPrecision", 0, 0).getUInt32();

    // clear the layout
    if(values.getArraySize() != m_widgets.size())
    {
      QLayoutItem* item;
      while ( ( item = m_listWidget->layout()->takeAt( 0 ) ) != NULL )
      {
        delete item->widget();
        delete item;
      }

      float bottom = m_param.callMethod("Float32", "getMin", 0, 0).getFloat32();
      float top = m_param.callMethod("Float32", "getMax", 0, 0).getFloat32();

      m_validator->setRange(bottom, top, precision);
      m_validator->setNotation(QDoubleValidator::StandardNotation);

      m_widgets.resize(values.getArraySize());
      for (unsigned int i = 0; i < values.getArraySize(); ++i)
      {
        m_widgets[i] = new ItemWidget(m_listWidget);
        m_widgets[i]->setFont( getApplicationWidgetFont() );
        m_widgets[i]->setValidator(m_validator);
    
        // connect the color box to uiChanged()
        connect( m_widgets[i], SIGNAL( textChanged(const QString &) ), this , SLOT( uiChanged() ) );
        m_listWidget->layout()->addWidget( m_widgets[i]);
      }
    }

    for (unsigned int i = 0; i < values.getArraySize(); ++i)
    {
      float value = values.getArrayElement(i).getFloat32();
      m_widgets[i]->setText(QString::number(value, 'g', precision));
    }

  );
}

void Float32AEWidget::uiChanged()
{
  FABRIC_TRY("Float32AEWidget::uiChanged", 

    FabricCore::RTVal values = getValueArray();
    m_param.callMethod("", "setValueArray", 1, &values);

  );

  AEWidget::uiChanged();
}