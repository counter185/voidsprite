package pl.cntrpl.voidsprite;

import org.libsdl.app.SDLActivity;

public class VSPActivity extends SDLActivity {

    @Override
    protected void main() {

        String appdataPath = getExternalFilesDir(null).getAbsolutePath() + "/";
        passAppdataPathString(appdataPath);

        super.main();
    }

    public static native void passAppdataPathString(String appdataPath);
}
