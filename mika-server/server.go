package main

import (
	"fmt"
	"html/template"
	"net/http"
	"time"
)

type PageData struct {
	Movements uint
}

func main() {
	var movements uint = 0

	// reset the movements every 24 hours
	DAY := 24 * 60 * 60 * 1000 * time.Millisecond

	ticker := time.NewTicker(DAY)

	go func() {
		for {
			<-ticker.C
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

	http.ListenAndServe(":8090", nil)
}
