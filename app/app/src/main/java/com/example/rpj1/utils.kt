package com.example.rpj1

import android.app.Activity
import android.bluetooth.BluetoothAdapter
import android.content.Intent
import android.content.pm.PackageManager
import android.util.Log
import androidx.core.app.ActivityCompat

import java.nio.ByteBuffer
import java.nio.ByteOrder


class utils {
    val myVars = vars()

    fun serializeFloat16Array(pyObject: Any): ByteArray {
        return (pyObject as? com.chaquo.python.PyObject)?.let { pythonObj ->
            // Get bytes directly from the numpy float16 array
            pythonObj.callAttr("tobytes")?.toJava(ByteArray::class.java)
        } ?: ByteArray(0)
    }

    fun calculate_azi(planet: String,latitude: Float,longtitude: Float,altitude: Float): ByteArray{ //  : Triple<Float, Float, Float>

        val lat1    :  Double
        val long1   :  Double
        val mag     :  Double


        val python = com.chaquo.python.Python.getInstance()

        val module = python.getModule("uhly")
        val result = module.callAttr("getCoordinateArray", planet, latitude,longtitude,altitude,0.03125f )

        val BytesToSend = serializeFloat16Array(result)


        return BytesToSend
    }

//#################################################################################################################################

    fun checkAndRequestPermissions(activity: Activity) {


        val notGrantedPermissions = myVars.permissions.filter {
            ActivityCompat.checkSelfPermission(activity, it) != PackageManager.PERMISSION_GRANTED
        }

        if (notGrantedPermissions.isNotEmpty()) {
            ActivityCompat.requestPermissions(activity, notGrantedPermissions.toTypedArray(), 1)
        }


        val enableBluetoothIntent = Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE)
        activity.startActivityForResult(enableBluetoothIntent, 1)

    }

    fun isobject(object_name: String): Boolean{
        return myVars.objects.contains(object_name.lowercase())
    }



}