import { isPlatformBrowser } from '@angular/common';
import { HttpClient } from '@angular/common/http';
import { inject, PLATFORM_ID, Service } from '@angular/core';
import { Observable } from 'rxjs';


type Callback<T> = (item: T) => void;

@Service()
export class HttpService {
    private readonly m_http = inject(HttpClient)
    private readonly m_platformID = inject(PLATFORM_ID);

    public delete<T>(path: string, withCredentials: boolean=true) : Observable<T> {
        if(isPlatformBrowser(this.m_platformID))
            return this.m_http.delete<T>(path, { withCredentials: withCredentials });

        return new Observable<T>();
    }

    public get<T>(path: string, withCredentials: boolean=true) : Observable<T> {
        if(isPlatformBrowser(this.m_platformID))
            return this.m_http.get<T>(path, { withCredentials: withCredentials });

        return new Observable<T>();
    }

    public patch<T, U>(path: string, body: T | null, withCredentials: boolean=true) : Observable<U> {
        if(isPlatformBrowser(this.m_platformID))
            return this.m_http.patch<U>(path, body, { withCredentials: withCredentials });

        return new Observable<U>();
    }

    public post<T, U>(path: string, body: T | null, withCredentials: boolean=true) : Observable<U> {
        if(isPlatformBrowser(this.m_platformID))
            return this.m_http.post<U>(path, body, { withCredentials: withCredentials });

        return new Observable<U>();
    }

    public put<T, U>(path: string, body: T | null, withCredentials: boolean=true) : Observable<U> {
        if(isPlatformBrowser(this.m_platformID))
            return this.m_http.put<U>(path, body, { withCredentials: withCredentials });

        return new Observable<U>();
    }
}
