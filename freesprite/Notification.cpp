#include "Notification.h"
#include "FontRenderer.h"
#include "background_operation.h"
std::vector<Notification> g_notifications;
bool renderingNotifications = false;

void g_renderNotifications()
{
    int notifY = 30;
    int notifOriginX = g_windowW - 450;
    renderingNotifications = true;
    for (auto it = g_notifications.rbegin(); it != g_notifications.rend(); it++) {
        Notification& notif = *it;

        bool compactNotification = (notif.title.empty() || notif.message.empty()) && notif.icon == NULL;

        //bounds
        int notifX = notifOriginX + 30 * (1.0 - XM1PW3P1(notif.timer.percentElapsedTime(300)));
        SDL_Rect r = { notifX, notifY, 400, compactNotification ? 30 : 60 };

        if (pointInBox({ g_mouseX, g_mouseY }, r)) {
            notif.timer.setElapsedTime(500);
        }

        //background
        SDL_SetRenderDrawColor(g_rd, 0, 0, 0, (uint8_t)(0xf0 * XM1PW3P1(notif.timer.percentElapsedTime(200) * (1.0 - notif.timer.percentElapsedTime(500, notif.duration - 500)))));
        SDL_RenderFillRect(g_rd, &r);

        //animated border lines
        //gradient
        uint32_t color = sdlcolorToUint32(notif.color);
        XY leftEP = statLineEndpoint(XY{ r.x, r.y }, XY{ r.x, r.y + r.h }, XM1PW3P1(notif.timer.percentElapsedTime(300)) * (1.0 - notif.timer.percentElapsedTime(500, notif.duration - 500)));
        renderGradient({ r.x, r.y, r.w / 4, leftEP.y - r.y }, modAlpha(color, 0x40), modAlpha(color, 0), modAlpha(color, 0x40), modAlpha(color, 0));

        SDL_SetRenderDrawColor(g_rd, notif.color.r, notif.color.g, notif.color.b, 0x80 + (uint8_t)(0x60 * (1.0 - XM1PW3P1(notif.timer.percentElapsedTime(500)))));
        //left line
        drawLine(XY{ r.x, r.y }, leftEP);
        //right line
        drawLine(XY{ r.x + r.w, r.y + r.h }, XY{ r.x + r.w, r.y }, XM1PW3P1(notif.timer.percentElapsedTime(300)) * (1.0 - notif.timer.percentElapsedTime(500, notif.duration - 500)));

        //top line [timer]
        XY topLeft = { notifX, notifY };
        XY topRight = { notifX + r.w, notifY };
        drawLine(topLeft, topRight, 1.0 - notif.timer.percentElapsedTime(notif.duration));

        //icon
        int textX = notifX + 10;
        if (notif.icon != NULL && notif.icon->get(g_rd) != NULL) {
            SDL_Texture* iconTex = notif.icon->get(g_rd);
            SDL_Rect iconRect = { notifX + 5, notifY + 5, 50, 50 };
            SDL_SetTextureAlphaMod(iconTex, (uint8_t)(0xff * XM1PW3P1(notif.timer.percentElapsedTime(200, 200)) * (1.0 - notif.timer.percentElapsedTime(500, notif.duration - 500))));
            SDL_RenderCopy(g_rd, iconTex, NULL, &iconRect);
            SDL_SetTextureAlphaMod(iconTex, 0xff);
            textX += 50;
        }

        //text
        int notifTextMidpoint = notifY + (compactNotification ? 3 : 15);
        int fontSize = compactNotification ? 16 : 18;
        g_fnt->RenderString(notif.title, textX, notif.message != "" ? notifY + 5 : notifTextMidpoint, SDL_Color{ 255,255,255,(uint8_t)(0xff * XM1PW3P1(notif.timer.percentElapsedTime(200, 100)) * (1.0 - notif.timer.percentElapsedTime(500, notif.duration - 500))) }, fontSize);
        g_fnt->RenderString(notif.message, textX, notif.title != "" ? notifY + 30 : notifTextMidpoint, SDL_Color{ 255,255,255,(uint8_t)(0xd0 * XM1PW3P1(notif.timer.percentElapsedTime(200, 150)) * (1.0 - notif.timer.percentElapsedTime(500, notif.duration - 500))) }, fontSize - 2);
        notifY += (r.h + 5) * XM1PW3P1(notif.timer.percentElapsedTime(300) * (1.0 - notif.timer.percentElapsedTime(500, notif.duration - 500)));

        //pulse outline
        if (g_config.vfxEnabled) {
            double timer800ms = notif.timer.percentElapsedTime(800);
            SDL_SetRenderDrawColor(g_rd, notif.color.r, notif.color.g, notif.color.b, (u8)(0x80 * (1.0 - XM1PW3P1(timer800ms))));
            SDL_Rect rg = offsetRect(r, 1 + 20 * XM1PW3P1(timer800ms));
            SDL_RenderDrawRect(g_rd, &rg);

            for (int x = 1; x < 6; x++) {
                XY vline1Origin = { r.x - (int)(40 * x * XM1PW3P1(timer800ms)), r.y };
                drawLine(vline1Origin, xyAdd(vline1Origin, { 0,r.h }), 1.0);
            }
        }
    }
    renderingNotifications = false;
}

void g_addNotification(Notification a) {
    if (!renderingNotifications) {
        g_notifications.push_back(a);
        loginfo(frmt("New notification:\n  {} | {}", a.title, a.message));
    }
    else {
        logerr("Failed to post notification (currently rendering notifications)");
    }
}
void g_addNotificationFromThread(Notification a) {
    g_startNewMainThreadOperation([a]() {
        g_addNotification(a);
    });
}
void g_tickNotifications() {
    for (int x = 0; x < g_notifications.size(); x++) {
        if (g_notifications[x].timer.elapsedTime() > g_notifications[x].duration) {
            g_notifications.erase(g_notifications.begin() + x);
            x--;
        }
    }
}