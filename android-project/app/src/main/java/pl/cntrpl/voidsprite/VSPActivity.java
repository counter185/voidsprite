package pl.cntrpl.voidsprite;

import android.net.Uri;
import android.os.Build;
import android.content.Intent;
import android.os.Environment;
import android.provider.Settings;

import org.libsdl.app.SDLActivity;

public class VSPActivity extends SDLActivity {

    @Override
    protected void main() {

        String appdataPath = getExternalFilesDir(null).getAbsolutePath() + "/";
        passAppdataPathString(appdataPath);

        String systemInformation = "Android " + android.os.Build.VERSION.RELEASE + " (" + android.os.Build.VERSION.SDK_INT + ")\n";
        systemInformation += String.format("%s %s | %s %s\n", Build.MANUFACTURER, Build.PRODUCT, Build.MODEL, Build.DEVICE);
        if (Build.VERSION.SDK_INT >= 31) {
            systemInformation += String.format("SOC: %s %s\n", Build.SOC_MANUFACTURER, Build.SOC_MODEL);
        }
        passSystemInformationString(systemInformation);

        // we need the all files access permission to support split sessions and all the obscure formats
        if (Build.VERSION.SDK_INT >= 30 && !Environment.isExternalStorageManager()) {
            Intent intent = new Intent(Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION);
            intent.setData(Uri.parse("package:" + getPackageName()));
            startActivity(intent);
        }
        
        super.main();
    }

    public static native void passAppdataPathString(String appdataPath);
    public static native void passSystemInformationString(String systemInformation);
}
