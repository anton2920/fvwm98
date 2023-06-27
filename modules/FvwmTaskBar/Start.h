#ifndef START_H
#define START_H

void StartButtonInit(int height);
void StartButtonUpdate(char *title, int state);
void StartButtonDraw(int force);
int  MouseInStartButton(int x, int y);

#endif /* START_H */
