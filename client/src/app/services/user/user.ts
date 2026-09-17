import { HttpErrorResponse } from '@angular/common/http';
import { inject, Service, signal } from '@angular/core';
import { Router } from '@angular/router';
import { environment } from '@environments/environment';
import { Login } from '@models/login';
import { Register } from '@models/register';
import { HttpService } from '@services/http/http';
import { catchError, of, tap } from 'rxjs';
import { routes } from '@config/app.routes';


type ErrorCallback = (error: HttpErrorResponse) => void;

@Service()
export class UserService {
    private readonly m_isAuthenticated = signal(false);
    private readonly m_http: HttpService = inject(HttpService);
    private readonly m_router: Router = inject(Router);

    constructor() {}

    public get isAuthenticated() {
        return this.m_isAuthenticated.asReadonly();
    }

    public authenticate() {
        return this.m_http.post(this.buildPath("/authenticate"))
                          .pipe(tap(this.handleAuthenticate.bind(this)),
                                catchError((error: HttpErrorResponse) => {
                                    if (error.status === 401)
                                        return this.refresh();

                                    this.m_isAuthenticated.set(false);
                                    this.redirect();
                                    return of(null);
                                }));
    }

    public refresh() {
        return this.m_http.post(this.buildPath("/auth/refresh")).pipe(tap(this.handleAuthenticate.bind(this)),
                                                                 catchError((error) => { this.handleRefreshError(error); return of(null); }));
    }

    public login(loginInformation: Login, callback: ErrorCallback | undefined=undefined) {
        this.m_http.post(this.buildPath("/login"), loginInformation).subscribe({ next: this.handleLogin.bind(this), error: callback });
    }

    public logout(callback: ErrorCallback | undefined=undefined) {
        if (this.isAuthenticated())
            this.m_http.post(this.buildPath("/auth/logout")).subscribe({ next: this.handleLogout.bind(this), error: callback });
    }

    public register(registerInformation: Register, callback: () => void, errorCallback: ErrorCallback | undefined=undefined) {
        this.m_http.post(this.buildPath("/register"), registerInformation).subscribe({ next: callback, error: errorCallback });
    }

    private buildPath(path: string) {
        return `${environment.apiUrl}/user${path}`;
    }

    private handleAuthenticate() {
        this.m_isAuthenticated.set(true);
    }

    private handleRefreshError(_: HttpErrorResponse) {
        this.m_isAuthenticated.set(false);
        this.redirect();
    }

    private handleLogin() {
        this.m_isAuthenticated.set(true);
        this.m_router.navigate(['/feed']);
    }

    private handleLogout() {
        this.m_isAuthenticated.set(false);
        this.m_router.navigate(['/']);
    }

    private redirect() {
        this.m_isAuthenticated.set(false);
        const KNOWN_PATHS = routes.map(r => r.path).filter(p => p !== '**' && p !== undefined);
        const currentPath = window.location.pathname.replace(/^\//, '');
        const isKnownRoute = KNOWN_PATHS.includes(currentPath);

        if (isKnownRoute)
            this.m_router.navigate(["/"]);
    }
}