/*************************************************/
/* methods for class SimW                        */
/*                                               */
/* window displaying logic signal flow           */
/*                                               */
/* Andreas Rostin                                */
/* 15.03.99                                      */
/*************************************************/
#include <QBitmap>
#include <qpainter.h>
#include <qmenu.h>
#include <qevent.h>
#include <qscrollbar.h>

#include "KlogicGlobal.h"
#include "OscContentWidget.h"
#include "circuit.h"
#include "Simulator.h"

#include "OscContentWidget.moc"


const QColor OscContentWidget::LT_GRAY_COLOR = QColor(248, 248, 248);
const QBrush OscContentWidget::LT_GRAY_BRUSH = QBrush(OscContentWidget::LT_GRAY_COLOR);

OscContentWidget::OscContentWidget(QWidget *parent)
	: QWidget(parent)
{
	setBackgroundRole(QPalette::Base);
	QPixmap csr_pic = QPixmap((const QPixmap)*(XDevice::m_oBasicImageCache.getImage(BasicImageCache::CURSOR_SIMWIDGET)));
    setMouseTracking(true);
}

OscContentWidget::~OscContentWidget() {
}

void OscContentWidget::displayCircuit(Circuit *circuit) {
	if (this->circuit == circuit) {
		circuitChanged(circuit);
	} else {
		channels.clear();
		this->circuit = circuit;
		circuitChanged(circuit);
	}
	repaint();
}

void OscContentWidget::circuitChanged(Circuit *circuit) {
	if (this->circuit == circuit) {
		qDebug() << "content of circuit changed";
		list<OscChannel *> newChannels;

		// Add the new channels
		int existingDepth = 0;
		for (DeviceList::iterator devIter = circuit->getDeviceIterator(); devIter != circuit->getDeviceEndIterator(); ++devIter) {
            XDevice *dev = devIter.getDevice();
            if (dev->isObserved()) {
				for (ConnectorList::iterator connIter = dev->getNamedOutputIterator(); connIter != dev->getNamedOutputEndIterator(); connIter++) {
					Connector& oConnector = *connIter;
					if (!oConnector.getCalculator()->isInternal() &&
							((dev->hasNamedOutput() && oConnector.getConnectorID() != 0) ||
							 (!dev->hasNamedOutput() && oConnector.getConnectorID() == 0)))	{
						bool found = false;
						for (list<OscChannel *>::iterator chIter = channels.begin(); chIter != channels.end(); ++chIter) {
							if ((*chIter)->equals(dev, oConnector.getConnectorID())) {
								newChannels.push_back(*chIter);
								existingDepth = (*chIter)->getValues().size();
								found = true;
								qDebug() << "channel " << (*chIter)->getName() << " kept in oscilloscope";
								break;
							}
						}
						if (!found) {
							OscChannel *channel = new OscChannel(dev, oConnector.getConnectorID(), existingDepth);
							channel->setSize(CHANNEL_SIZE);
							newChannels.push_back(channel);
							channels.push_back(channel);
							qDebug() << "channel " << channel->getName() << " added to oscilloscope";
						}
					}
				}
            }
		}
		// Delete the channels not existent anymore
		list<OscChannel *>::iterator chIter = channels.begin();
		while (chIter != channels.end()) {
			bool found = false;
			for (list<OscChannel *>::iterator chIter2 = newChannels.begin(); chIter2 != newChannels.end(); chIter2++) {
				if (*chIter == *chIter2) {
					found = true;
					break;
				}
			}
			if (!found) {
				qDebug() << "channel " << (*chIter)->getName() << " removed from oscilloscope";
				delete *chIter;
				chIter = channels.erase(chIter);
			} else {
				chIter++;
			}
		}

	}
	repaint();
}

void OscContentWidget::simStep() {
	if (Simulator::getInstance()->isPaused()) {
		return;
	}
	for (list<OscChannel *>::iterator iter = channels.begin(); iter != channels.end(); iter++) {
		OscChannel *channel = *iter;
		channel->simStep();
	}
	repaint();
}

void OscContentWidget::mousePressEvent(QMouseEvent *) {
}

void OscContentWidget::mouseMoveEvent(QMouseEvent *e) {
	currentMouseXPos = e->x();
	repaint();
}

void OscContentWidget::mouseReleaseEvent(QMouseEvent *) {
}

void OscContentWidget::paintEvent(QPaintEvent *) {
	QPainter p;
	p.begin(this);
	p.setFont(KlogicGlobal::Device::m_oFont);

	int channelIdx = 1;
	for (list<OscChannel *>::iterator chIter = channels.begin(); chIter != channels.end(); ++chIter) {
		int channelYPos = channelIdx * ROW_HEIGHT;
		int channelYHeight = ROW_HEIGHT;

		if (height() < channelYPos + ROW_HEIGHT) {
			resize(width(), channelYPos + ROW_HEIGHT);
		}

		p.setBrush(QBrush(OscContentWidget::LT_GRAY_BRUSH));
		p.setPen(OscContentWidget::LT_GRAY_COLOR);
		p.drawRect(QRect(CHANNEL_START, channelYPos, CHANNEL_SIZE, CHANNEL_HEIGHT));

		OscChannel *channel = *chIter;

		p.setPen(Qt::black);
		p.drawText(5, channelYPos + ROW_HEIGHT - CHANNEL_HEIGHT_HALF, channel->getName());

		const list<int> &values =  channel->getValues();
		int xPosOfValue = CHANNEL_START;
		int prevValue = KlogicGlobal::Device::TRISTATE;
		int yposOfPrevValue = -1;
		for (list<int>::const_iterator iter = values.begin(); iter != values.end(); ++iter) {
			int value = *iter;
			if (value != KlogicGlobal::Device::TRISTATE) {
				int yposOfValue = (value == 0) ? channelYPos + channelYHeight - 3: channelYPos + 1;
				if (prevValue != KlogicGlobal::Device::TRISTATE && prevValue != value) {
					p.drawLine(xPosOfValue, yposOfValue, xPosOfValue, yposOfPrevValue);
				} else {
					p.drawPoint(xPosOfValue, yposOfValue);
				}
				yposOfPrevValue = yposOfValue;
			}
			prevValue = value;
			++xPosOfValue;
		}

		channelIdx++;
	}

	QString stepText = "step";
	p.setPen(Qt::black);
	p.drawText(CHANNEL_START, 20, stepText);
	for (int i = CHANNEL_START + CHANNEL_SIZE; i >= 50; i -= 100) {
		p.setPen(QPen(Qt::black, 1, Qt::DotLine));
		p.drawLine(i, ROW_HEIGHT, i, height());

		p.setPen(Qt::black);
		int step = WIDTH - 10 - i;
		stepText.sprintf("%d", step);
		p.drawText(i - 8, 20, stepText);
	}

	if (currentMouseXPos >= 50 && currentMouseXPos <= WIDTH - 10) {
		p.setPen(QPen(Qt::black, 1, Qt::DashLine));
		p.drawLine(currentMouseXPos, ROW_HEIGHT, currentMouseXPos, height());

		p.setPen(Qt::black);
		int step = WIDTH - 10 - currentMouseXPos;
		stepText.sprintf("step %d", step);
		p.drawText(currentMouseXPos - 40, 10, stepText);
	}

}

