import { HttpErrorResponse } from '@angular/common/http';
import { inject, OnDestroy, Service, signal } from '@angular/core';
import { Router } from '@angular/router';
import { GenericSubscriber } from '@classes/generic-subscriber/subscriber';
import { environment } from '@environments/environment';
import { Login } from '@models/login';
import { Register } from '@models/register';
import { Subscription } from '@models/subscription';
import { HttpService } from '@services/http/http';
import { PublisherService } from '@services/publisher/publisher';
import { catchError, EMPTY, finalize, Observable, of, tap } from 'rxjs';
import { routes } from '@config/app.routes';


type ErrorCallback = (error: HttpErrorResponse) => void;

@Service()
export class UserService implements OnDestroy {
    private readonly m_isAuthenticated = signal(false);
    private readonly m_http: HttpService = inject(HttpService);
    private readonly m_publisher: PublisherService = inject(PublisherService);
    private readonly m_router: Router = inject(Router);
    private readonly m_subscribed: Subscription[] = [];
    private static m_isHandlingAuthError: boolean = false;

    constructor() {
        this.m_subscribed.push(this.m_publisher.subscribe("unauthorized", GenericSubscriber.create((data: object) => {
            if (!UserService.m_isHandlingAuthError)
                this.handleAuthenticationError(data as HttpErrorResponse).subscribe();
        })));
    }

    public ngOnDestroy(): void {
        this.m_subscribed.forEach((subscription) => {
            this.m_publisher.unsubscribe(subscription.event, subscription.subscriber);
        });
    }

    public get isAuthenticated() {
        return this.m_isAuthenticated.asReadonly();
    }

    public authenticate() {
        return this.m_http.post(this.buildPath("/authenticate"))
                          .pipe(tap(this.handleAuthenticate.bind(this)),
                                catchError(this.handleAuthenticationError.bind(this)));
    }

    public refresh() {
        return this.m_http.post<null>(this.buildPath("/auth/refresh")).pipe(tap(this.handleAuthenticate.bind(this)),
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

    private handleAuthenticationError(error: HttpErrorResponse): Observable<null> {
        if (UserService.m_isHandlingAuthError)
            return EMPTY;

        UserService.m_isHandlingAuthError = true;
        if (error.status === 401)
            return this.refresh().pipe(finalize(() => UserService.m_isHandlingAuthError = false));

        UserService.m_isHandlingAuthError = false;
        this.m_isAuthenticated.set(false);
        this.redirect();
        return EMPTY;
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