package main

import (
	"fmt"
	"html/template"
	"log"
	"net/http"
	"os"
	"time"
)

type PageData struct {
	Movements uint
}

func main() {
	var movements uint = 0

	// reset the movements every midnight
    go func() {
        now := time.Now()
        midnight := time.Date(now.Year(), now.Month(), now.Day(),
                                23, 59, 59, 999, now.Location())
        nowToMidnight := midnight.Sub(now)
        for {
            time.Sleep(nowToMidnight)
            nowToMidnight = 24 * time.Hour
            movements = 0
        }
    }()

	// adds 1 to the movements
	http.HandleFunc("/add", func(w http.ResponseWriter, _ *http.Request) {
		movements += 1
		w.WriteHeader(http.StatusOK)
	})

	// generates the html page with the movement information
	// if it fails to open template, it just prints the movement number
	tmpl, tmplErr := template.ParseFiles("index.html")

	http.HandleFunc("/", func(w http.ResponseWriter, _ *http.Request) {
		if tmplErr != nil {
			fmt.Fprintf(w, "%d", movements)
			return
		}
		data := PageData{
			Movements: movements,
		}
		tmpl.Execute(w, data)
	})

    port := getPort()
    log.Print("Listening on: " + port)
	log.Fatal(http.ListenAndServe(":"+port, nil))
}

func getPort() string {
    port := os.Getenv("PORT")
    if port == "" {
        port = "8090"
        log.Print("INFO: Unable to find port environment variable, defaulting to " + port)
    }
    return port
}
