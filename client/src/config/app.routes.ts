import { Routes } from '@angular/router';
import { AuthenticationPage } from '@components/authentication/authentication'
import { FeedPage } from '@components/feed/feed';
import { authGuard } from '@guards/auth/auth-guard';
import { authRedirectGuard } from '@guards/auth-redirect/auth-redirect-guard';

export const routes: Routes = [{ path: "",     component: AuthenticationPage, canActivate: [authRedirectGuard] },
                               { path: "feed", component: FeedPage,           canActivate: [authGuard] }];
