#ifndef OPTIDECK_THEMEIMAGEPROVIDER_H
#define OPTIDECK_THEMEIMAGEPROVIDER_H

#include <QIcon>
#include <QPixmap>
#include <QQuickImageProvider>

class ThemeImageProvider : public QQuickImageProvider {
public:
    ThemeImageProvider() : QQuickImageProvider(QQuickImageProvider::Pixmap) {
    }

    QPixmap requestPixmap(const QString& id, QSize* size, const QSize& requestedSize) override {
        // id corresponds to the icon name, e.g. "camera-web" or "input-mouse"
        const QIcon icon = QIcon::fromTheme(id);

        const int width = (requestedSize.width() > 0) ? requestedSize.width() : 64;
        const int height = (requestedSize.height() > 0) ? requestedSize.height() : 64;

        if (size) {
            *size = QSize(width, height);
        }

        return icon.pixmap(width, height);
    }
};

#endif // OPTIDECK_THEMEIMAGEPROVIDER_H
