package main
// #include <stdlib.h>
/*
#include "main.h"
 */
import "C"
import (
	"encoding/json"
	"errors"
	"log"
)
// Thank you for https://github.com/Demion/nvapioc
func main() {
	d := &NvapiStruct{}
	err := d.Initialize()
	if err != nil {
		log.Println(err)
	}

	data, err := d.GetGpuinfo()
	if err != nil {
		log.Println(err)
	}

	log.Println("result",data)
}

type NvapiGpuinfo struct {
	Name string `json:"name"`
	Maker int `json:"maker"`
	Type int `json:"type"`
}
type StringNvapiGpuinfo struct {
	Name string `json:"name"`
	Maker string `json:"maker"`
	Type string `json:"type"`
}

type NvapiStruct struct {
	isInitialize bool
	BusId uint
	gpuinfo *C.char
}

func (n *NvapiStruct) Initialize() error {
	if (n.isInitialize) { return errors.New("Already initialized") }
	if (!C.Initialize()) { return errors.New("Nvapi Initialize Faild") }
	if (!C.EnumGpus()) { return errors.New("Nvapi NvAPI_EnumPhysicalGPUs Faild") }
	busid := C.uint(0)
	if (!C.GetBusId(busid, C.int(0))) { return errors.New("Nvapi NvAPI_GPU_GetBusId Faild") }
	n.BusId = uint(busid)
	n.isInitialize = true
	n.gpuinfo = C.CString("")
	return nil
}

func (n *NvapiStruct) GetGpuinfo() (StringNvapiGpuinfo, error) {
	var result StringNvapiGpuinfo
	if (!n.isInitialize) { return result, errors.New("need initialized") }
	if (!C.GetGpuInfo(n.gpuinfo, 0)) { return result, errors.New("Nvapi NvAPI_GPU_GetBusId Faild") }
	jsondata := C.GoString(n.gpuinfo)
	var nvapigpuinfo NvapiGpuinfo
	err := json.Unmarshal([]byte(jsondata), &nvapigpuinfo)
	if err != nil {
		return result, err
	}
	result.Maker = n.getGpuinfo_maker(nvapigpuinfo)
	result.Type = n.getGpuinfo_type(nvapigpuinfo)
	result.Name = nvapigpuinfo.Name
	return result, nil
}

func (n *NvapiStruct) getGpuinfo_maker(d NvapiGpuinfo) (string) {
	switch d.Maker {
	case 0:
		return "None"
	case 1:
		return "Samsung"
	case 2:
		return "QImonda"
	case 3:
		return "Elpida"
	case 4:
		return "Etron"
	case 5:
		return "Nanya"
	case 6:
		return "Hynix"
	case 7:
		return "Mosel"
	case 8:
		return "Winbond"
	case 9:
		return "Elite"
	case 10:
		return "Micron"
	default:
		return "None"
	}
}
func (n *NvapiStruct) getGpuinfo_type(d NvapiGpuinfo) (string) {
	switch d.Type {
	case 1:
		return "SDRAM"
	case 2:
		return "DDR1"
	case 3:
		return "DDR2"
	case 4:
		return "GDDR2"
	case 5:
		return "GDDR3"
	case 6:
		return "GDDR4"
	case 7:
		return "DDR3"
	case 8:
		return "GDDR5"
	case 9:
		return "LPDDR2"
	case 10:
		return "GDDR5X"
	default:
		return "Unkown"
	}
}
