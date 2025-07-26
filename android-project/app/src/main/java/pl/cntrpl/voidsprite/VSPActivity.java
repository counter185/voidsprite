package pl.cntrpl.voidsprite;

import android.Manifest;
import android.app.Application;
import android.net.Uri;
import android.os.Build;
import android.content.Intent;
import android.os.Environment;
import android.provider.Settings;

import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.core.content.PermissionChecker;

import java.net.URI;

import org.libsdl.app.SDLActivity;

import pl.cntrpl.voidsprite.util.Utils;

public class VSPActivity extends SDLActivity {

    public static String packageName = "";
    public static VSPActivity activitySingleton = null;

    @Override
    protected void main() {

        packageName = getPackageName();
        activitySingleton = this;

        String appdataPath = getExternalFilesDir(null).getAbsolutePath() + "/";
        passAppdataPathString(appdataPath);

        String systemInformation = "Android " + android.os.Build.VERSION.RELEASE + " (" + android.os.Build.VERSION.SDK_INT + ")\n";
        systemInformation += String.format("%s %s | %s %s\n", Build.MANUFACTURER, Build.PRODUCT, Build.MODEL, Build.DEVICE);
        if (Build.VERSION.SDK_INT >= 31) {
            systemInformation += String.format("SOC: %s %s\n", Build.SOC_MANUFACTURER, Build.SOC_MODEL);
        }
        passSystemInformationString(systemInformation);

        // we need the all files access permission to support split sessions and all the obscure formats
        /*if (Build.VERSION.SDK_INT >= 30 && !Environment.isExternalStorageManager()) {
            Intent intent = new Intent(Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION);
            intent.setData(Uri.parse("package:" + getPackageName()));
            startActivity(intent);
        }*/
        
        super.main();
    }

    public static boolean hasFileAccessPermission() {
        return Build.VERSION.SDK_INT >= 30 ? Environment.isExternalStorageManager()
                : (ContextCompat.checkSelfPermission(activitySingleton, Manifest.permission.WRITE_EXTERNAL_STORAGE) == PermissionChecker.PERMISSION_GRANTED);
    }

    public static void requestAllFilesPermission() {
        if (Build.VERSION.SDK_INT >= 30) {
            Intent intent = new Intent(Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION);
            intent.setData(Uri.parse("package:" + packageName));
            activitySingleton.startActivity(intent);
        } else {
            ActivityCompat.requestPermissions(activitySingleton, new String[] { Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE }, 1);
        }
    }

    public static void openUrl(String url) {
        Intent intent = new Intent(Intent.ACTION_VIEW);
        intent.setData(Uri.parse(url));
        activitySingleton.startActivity(intent);
    }

    public static String fetchStringHTTP(String url) {
        return Utils.fetchStringHTTP(url);
    }

    public static native void passAppdataPathString(String appdataPath);
    public static native void passSystemInformationString(String systemInformation);
}
