package pl.cntrpl.voidsprite;

import android.app.Application;
import android.net.Uri;
import android.os.Build;
import android.content.Intent;
import android.os.Environment;
import android.provider.Settings;

import androidx.core.content.PermissionChecker;

import java.net.URI;

import org.libsdl.app.SDLActivity;

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
        return Build.VERSION.SDK_INT < 30 || Environment.isExternalStorageManager();
    }

    public static void requestAllFilesPermission() {
        if (Build.VERSION.SDK_INT >= 30) {
            Intent intent = new Intent(Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION);
            intent.setData(Uri.parse("package:" + packageName));
            activitySingleton.startActivity(intent);
        }
    }

    public static void openUrl(String url) {
        Intent intent = new Intent(Intent.ACTION_VIEW);
        intent.setData(Uri.parse(url));
        activitySingleton.startActivity(intent);
    }

    public static String fetchStringHTTP(String url) {
        try {
            java.net.URL urlObj = new URI(url).toURL();
            java.net.HttpURLConnection connection = (java.net.HttpURLConnection) urlObj.openConnection();
            connection.setRequestMethod("GET");
            connection.setConnectTimeout(5000);
            connection.setReadTimeout(5000);
            connection.connect();

            if (connection.getResponseCode() == 200) {
                java.io.BufferedReader reader = new java.io.BufferedReader(new java.io.InputStreamReader(connection.getInputStream()));
                StringBuilder response = new StringBuilder();
                String line;
                while ((line = reader.readLine()) != null) {
                    response.append(line).append("\n");
                }
                reader.close();
                return response.toString().trim();
            } else {
                return null;
            }
        } catch (Exception e) {
            return null;
        }
    }

    public static native void passAppdataPathString(String appdataPath);
    public static native void passSystemInformationString(String systemInformation);
}
