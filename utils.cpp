#include "utils.hpp"


double max(std::vector<double> &v){
    double maxi = abs(v[0]);
    for(unsigned long i=0;i<v.size();i++){
        if (abs(v[i])> maxi){
            maxi = abs(v[i]);
        }
    }
    return maxi;
}

void drawLine(sf::RenderWindow& window, int x1, int y1, int x2, int y2, sf::Color color, float thickness)
{
    // Calculate the length of the line
    float length = std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));

    // Calculate the angle of the line
    float angle = std::atan2(y2 - y1, x2 - x1) * 180 / M_PI;

    // Create a rectangle with the given thickness and length
    sf::RectangleShape line(sf::Vector2f(length, thickness));
    line.setFillColor(color);

    // Set the origin of the rectangle to the left side (0, thickness / 2)
    line.setOrigin(0, thickness / 2);

    // Position and rotate the rectangle to match the line
    line.setPosition(x1, y1);
    line.setRotation(angle);

    window.draw(line);
}

int createQtInterface(int argc, char *argv[], double *a, double *r, double *k, double *t, double *dt){
    QApplication app(argc,argv);

    QWidget window;
    window.setWindowTitle("Simple Qt Interface");
    window.setFixedSize(600, 400); // Set a fixed size for the window

    // Load the background image
    QPixmap originalImage("bg.jpg");
    // Scale the image to fit the window size
    QPixmap backgroundImage = originalImage.scaled(window.size(), Qt::KeepAspectRatioByExpanding);

    // Set the background image for the window
    QPalette palette;
    palette.setBrush(QPalette::Background, backgroundImage);
    window.setPalette(palette);

    QVBoxLayout *layout = new QVBoxLayout(&window);

    QHBoxLayout *hLayout = new QHBoxLayout; // Horizontal layout

    QFormLayout *formLayout1 = new QFormLayout;
    QLineEdit *aEdit = new QLineEdit(QString::number(*a));
    QLineEdit *rEdit = new QLineEdit(QString::number(*r));
    formLayout1->addRow(new QLabel("a:"), aEdit);
    formLayout1->addRow(new QLabel("r:"), rEdit);

    QFormLayout *formLayout2 = new QFormLayout;
    QLineEdit *kEdit = new QLineEdit(QString::number(*k));
    QLineEdit *tEdit = new QLineEdit(QString::number(*t));
    formLayout2->addRow(new QLabel("K_:"), kEdit);
    formLayout2->addRow(new QLabel("T:"), tEdit);

    QFormLayout *formLayout3 = new QFormLayout;
    QLineEdit *dtEdit = new QLineEdit(QString::number(*dt));
    formLayout3->addRow(new QLabel("dt:"), dtEdit);

    // Increase the font size of the text elements
    QFont font = aEdit->font();
    font.setPointSize(14);
    aEdit->setFont(font);
    rEdit->setFont(font);
    kEdit->setFont(font);
    tEdit->setFont(font);
    dtEdit->setFont(font);

    // Add empty spacer in the center of the 3rd row
    QSpacerItem* spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    formLayout3->addItem(spacer);

    hLayout->addLayout(formLayout1);
    hLayout->addLayout(formLayout2);
    hLayout->addLayout(formLayout3);

    layout->addLayout(hLayout); // Add the horizontal layout to the main layout

    QPushButton *button = new QPushButton("Calculate");
    layout->addWidget(button);

    QObject::connect(button, &QPushButton::clicked, [&](){
        *a = aEdit->text().toDouble();
        *r = rEdit->text().toDouble();
        *k = kEdit->text().toDouble();
        *t = tEdit->text().toDouble();
        *dt = dtEdit->text().toDouble();
        window.close(); // Close the Qt interface
    });

    window.setLayout(layout);
    window.show();

    return app.exec();

}

void drawProgressBar(sf::RenderWindow& window, float currentProgress, int finalIndex)
{
    window.clear();
    sf::Font font;
    font.loadFromFile("arial.ttf");
    sf::Text text("Calcul du schema numerique", font, 40); // Label text and font size
    text.setPosition(375,300); // Position the label at the end of the x-axis
    window.draw(text);

    // Define the progress bar
    sf::RectangleShape progressBar(sf::Vector2f(400, 30));
    progressBar.setFillColor(sf::Color::Green);
    progressBar.setPosition(435, 400);

    // Define the progress indicator
    sf::RectangleShape progressIndicator(sf::Vector2f(0, 30));
    progressIndicator.setFillColor(sf::Color::Red);
    progressIndicator.setPosition(435, 400);
    //draw progress percentage
    std::string textString = std::to_string((int) currentProgress*100/finalIndex);
    textString+="%";
    sf::Text percentageText(textString,font,15);
    // Calculate the total number of steps
    int totalSteps = finalIndex;

    // Update the progress indicator
    float progressWidth = (currentProgress / totalSteps) * 400;
    progressIndicator.setSize(sf::Vector2f(progressWidth, 30));
    float pos = (400+progressWidth >435 ) ? 400+progressWidth : 435;  
    percentageText.setPosition(pos,405);

    // Draw the progress bar and indicator
    window.draw(progressBar);
    window.draw(progressIndicator);
    window.draw(percentageText);

    // Display the window contents
    window.display();

    // Handle events
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window.close();
    }
}
