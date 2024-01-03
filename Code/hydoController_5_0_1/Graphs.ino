// The scaling line graph function
void drawGraph (
  const int& a_xStartPos, const int& a_yStartPos, const int& a_graphHeight,
  const int& a_graphWidth, const int& a_numYLabels, float** a_data, 
  const int a_numArrays, const int& a_arrayNumber, const int& a_percision, const unsigned int* a_colors)
{
  float minArrayVal = 999, maxArrayVal = -999;
  tft.setFontScale(1);
  tft.setFont(&myriadPro_32px_Regular);
  tft.setTextColor(RA8875_BLACK);

  // work out the greatest number to display on the y axis.
  for (int i = 0; i < a_numArrays; i++) {
    for (int n = 0; n < a_arrayNumber; n++) {
      if (a_data[i][n] > maxArrayVal)
       maxArrayVal = a_data[i][n];
      if (a_data[i][n] < minArrayVal)
       minArrayVal = a_data[i][n];
    }
  }
  
  bool redrawGraph = device::newGraphData;
  static float graphXstartPosition = a_xStartPos;
  static float xSpacing = 0;
  static float prevMinArrayVal = 999, prevMaxArrayVal = -999;
  if (display::refreshPage || hasChanged(minArrayVal, prevMinArrayVal, 0.01) || hasChanged(maxArrayVal, prevMaxArrayVal, 0.01)) {
    tft.fillRect(a_xStartPos, a_yStartPos, a_graphWidth, 480 - a_yStartPos, user::backgroundColor); //
    // work out the x position to start the graph at, based on the width on the y axis numbers. while drawing the draw y line and numbers   
    float yIncrement = maxArrayVal / a_numYLabels;
    float yLablePosition = a_yStartPos - 6;
    float tempYmax = maxArrayVal;
    const float yLableGap = (a_graphHeight - 10) / a_numYLabels;
    graphXstartPosition = a_xStartPos;
    for (int i = 0; i < a_numYLabels + 1; i++) {
      tft.setCursor(a_xStartPos, yLablePosition);
      tft.print(tempYmax, a_percision);
      yLablePosition += (a_graphHeight - 10) / a_numYLabels;
      tempYmax -= yIncrement;
      if (tft.getFontX() > graphXstartPosition)
        graphXstartPosition = tft.getFontX();
    }   
    tft.drawLine(graphXstartPosition + 1, a_yStartPos + a_graphHeight, graphXstartPosition + 1, a_yStartPos, RA8875_BLACK);

    // draw x line and numbers
    float xLablePosition = graphXstartPosition + 2;
    xSpacing = (float)(a_graphWidth - (graphXstartPosition - a_xStartPos)) / device::maxGraphArrayValues;
    float lableSpacing = (float)(a_graphWidth - (graphXstartPosition - a_xStartPos)) / ((device::maxGraphArrayValues / 10) + 1);
    for (int i = 0; i < (device::maxGraphArrayValues / 10) + 1; i++) {
      tft.setCursor(xLablePosition, a_yStartPos + a_graphHeight - 1);
      tft.print(i*10);
      xLablePosition += lableSpacing;
    }
    int lineEnd = tft.getFontX();
    tft.drawLine(graphXstartPosition + 2, a_yStartPos + a_graphHeight, lineEnd, a_yStartPos + a_graphHeight, RA8875_BLACK);
    redrawGraph = true;
    prevMinArrayVal = minArrayVal;
    prevMaxArrayVal = maxArrayVal;
  }
  else if (redrawGraph) {
    tft.fillRect(graphXstartPosition + 2, a_yStartPos, a_graphWidth - (graphXstartPosition - a_xStartPos), a_graphHeight, user::backgroundColor);
  }

  // draw line graph
  if (redrawGraph) { // display::refreshPage || 
    if (a_arrayNumber >= 2) {
      for (int i = 0; i < a_numArrays; i++) {
        float yGap = a_graphHeight / maxArrayVal;
        float xGap = graphXstartPosition + 2;
        float plotYstartPosition = 0, plotYendPosition = 0;
        for (int n = 0; n < a_arrayNumber - 1; n++) {
          plotYstartPosition = mapFloat(a_data[i][n], minArrayVal, maxArrayVal, a_yStartPos + a_graphHeight - 1, a_yStartPos);
          plotYendPosition = mapFloat(a_data[i][n+1], minArrayVal, maxArrayVal, a_yStartPos + a_graphHeight - 1, a_yStartPos);
          tft.drawLine(xGap, plotYstartPosition, xGap + xSpacing, plotYendPosition, a_colors[i]);
          xGap += xSpacing;
        }
      }
    }
  }

}