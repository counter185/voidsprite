package pl.cntrpl.voidsprite.util;

import java.io.File;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.net.URI;
import java.util.List;

public class Utils {
    //backported from openjdk 9 code because old android sdk doesn't have this
    public static int j9_readNBytes(InputStream stm, byte[] b, int off, int len) throws IOException {

        int n = 0;
        while (n < len) {
            int count = stm.read(b, off + n, len - n);
            if (count < 0)
                break;
            n += count;
        }
        return n;
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

    public static byte[] fetchDataHTTP(String url) {
        try {
            java.net.URL urlObj = new URI(url).toURL();
            java.net.HttpURLConnection connection = (java.net.HttpURLConnection) urlObj.openConnection();
            connection.setRequestMethod("GET");
            connection.setConnectTimeout(5000);
            connection.setReadTimeout(5000);
            connection.connect();

            if (connection.getResponseCode() == 200) {
                java.io.InputStream inputStream = connection.getInputStream();
                int dataSize = connection.getContentLength();
                byte[] data = new byte[dataSize];
                j9_readNBytes(inputStream, data, 0, dataSize);
                return data;
            } else {
                return null;
            }
        } catch (Exception e) {
            return null;
        }
    }

    public static void writeU32(FileOutputStream out, int u32) {
        byte[] bytes = new byte[4];
        bytes[0] = (byte) (u32 & 0xFF);
        bytes[1] = (byte) ((u32 >> 8) & 0xFF);
        bytes[2] = (byte) ((u32 >> 16) & 0xFF);
        bytes[3] = (byte) ((u32 >> 24) & 0xFF);
        try {
            out.write(bytes);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static boolean writeVOIDPLTv1(File outFile, List<Integer> colorList) {
        try {
            FileOutputStream fos = new FileOutputStream(outFile);

            String header = "VOIDPLT\1";
            fos.write(header.getBytes("ASCII"));
            writeU32(fos, colorList.size());
            for (Integer color : colorList) {
                color |= 0xFF000000;
                writeU32(fos, color);
            }
            fos.close();

            return true;
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
    }
}
